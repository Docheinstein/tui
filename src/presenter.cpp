#include "presenter.h"
#include "block.h"
#include "hlayout.h"
#include "vlayout.h"
#include <iostream>

namespace Tui {
Presenter::Presenter(std::ostream& os) :
    os(os) {
}

struct NodePresentation {
    explicit NodePresentation(Node& node, NodePresentation* parent) :
        node(node),
        parent(parent) {
    }

    Node& node;
    NodePresentation* parent {};
    bool done {};
};

struct BlockPresentation : NodePresentation {
    BlockPresentation(Block& node, NodePresentation* parent) :
        NodePresentation(node, parent),
        node(node) {
    }

    Block& node;
    uint32_t width {};
    uint32_t line {};
    bool endl {};
};

struct ContainerPresentation : NodePresentation {
    ContainerPresentation(Container& node, NodePresentation* parent) :
        NodePresentation(node, parent),
        node(node) {
    }

    Container& node;
    std::vector<std::unique_ptr<NodePresentation>> children;
};

struct HLayoutPresentation : ContainerPresentation {
    HLayoutPresentation(HLayout& node, NodePresentation* parent) :
        ContainerPresentation(node, parent),
        node(node) {
    }

    HLayout& node;
};

struct VLayoutPresentation : ContainerPresentation {
    VLayoutPresentation(VLayout& node, NodePresentation* parent) :
        ContainerPresentation(node, parent),
        node(node) {
    }

    VLayout& node;
};

void Presenter::present(std::unique_ptr<Node>&& rootNode) {
    std::unique_ptr<NodePresentation> root;

    /*
     * Example of a layout with the associated tree.
     *
     * <--------- H1 --------->
     *
     *            <---- H2 --->
     *
     * +----------+------+----+   <+     <+
     * |          |  B2  | B3 |    |      |
     * |          +-----------+    |      |
     * |    B1    |           |    | V2   |
     * |          |     B4    |    |      |
     * |          |           |    |      | V1
     * +----------+-----------+   <+      |
     * |         B5           |           |
     * +----------------------+          <+
     *
     *
     *              V1
     *            /    \
     *          H1      B5      <----+
     *        /   \                  |
     *      B1     V2                |
     *           /    \              |  ending blocks
     *         H2      B4       <----+
     *       /   \                   |
     *     B2     B3            <----+
     */

    // 1) First visit of the tree.
    //    - Wrap each node with a presentation node (wrapper that adds helpers).
    //    - Do some preprocessing.
    {
        const auto makeNodePresentation = [](Node& node,
                                             NodePresentation* parent) -> std::unique_ptr<NodePresentation> {
            if (node.type == Node::Type::Block) {
                return std::make_unique<BlockPresentation>(static_cast<Block&>(node), parent);
            } else if (node.type == Node::Type::HLayout) {
                return std::make_unique<HLayoutPresentation>(static_cast<HLayout&>(node), parent);
            } else if (node.type == Node::Type::VLayout) {
                return std::make_unique<VLayoutPresentation>(static_cast<VLayout&>(node), parent);
            }
            return nullptr;
        };

        root = makeNodePresentation(*rootNode, nullptr);

        std::vector<NodePresentation*> stack;
        stack.push_back(&*root);

        while (!stack.empty()) {
            NodePresentation* node = stack.back();
            stack.pop_back();

            if (node->node.type == Node::Type::Block) {
                // Compute block properties
                auto* b = static_cast<BlockPresentation*>(node);

                // Eventually strip out empty new lines
                for (int32_t i = static_cast<int32_t>(b->node.lines.size()) - 1; i >= 0; i--) {
                    if (b->node.lines[i].empty())
                        b->node.lines.pop_back();
                }

                // Compute block's width
                if (b->node.width) {
                    // Fixed width
                    b->width = *b->node.width;
                } else {
                    // Variable width
                    for (const auto& l : b->node.lines) {
                        b->width = std::max(b->width, l.size());
                    }
                }

            } else if (node->node.type == Node::Type::HLayout || node->node.type == Node::Type::VLayout) {
                // Wrap each child into a presentation node
                auto* c = static_cast<ContainerPresentation*>(node);
                c->children.resize(c->node.children.size());
                for (uint32_t i = 0; i < c->children.size(); i++) {
                    c->children[i] = makeNodePresentation(*c->node.children[i], node);
                    stack.push_back(&*c->children[i]);
                }
            }
        }
    }

    // 2) Find ending blocks (the ones at the right of the entire content)
    //    An ending block is defined by the following rule:
    //    A block is an ending block unless it is not part of the rightmost
    //    branch of the Horizontal Layout ancestors it has.
    {
        std::vector<NodePresentation*> stack {&*root};

        while (!stack.empty()) {
            NodePresentation* node = stack.back();
            stack.pop_back();

            if (node->node.type == Node::Type::Block) {
                static_cast<BlockPresentation*>(node)->endl = true;
            }
            if (node->node.type == Node::Type::HLayout) {
                const auto* h = static_cast<const HLayoutPresentation*>(node);
                if (!h->children.empty()) {
                    // Only the blocks of the right most branch are candidates.
                    stack.push_back(&*h->children.back());
                }
            } else if (node->node.type == Node::Type::VLayout) {
                const auto* v = static_cast<const VLayoutPresentation*>(node);
                for (const auto& n : v->children) {
                    stack.push_back(&*n);
                }
            }
        }
    }

    // 3) Presentation.
    //    The logic is the following:
    //    - Horizontal Layout:
    //          Processed in parallel.
    //          When it is visited all its children are visited too.
    //    - Vertical Layout
    //          Processed sequentially.
    //          When it is visited only the first child for which the render
    //          is not finished is visited.
    //    - Block
    //          When it is visited it pushes the next line to the output stream.
    //          The lines are truncated/expanded to exactly fill the block's width.
    //          If there are no more lines to render,  it pushes empty lines
    //          to fill the block's width.
    do {
        std::vector<NodePresentation*> stack {&*root};

        while (!stack.empty()) {
            NodePresentation* node = stack.back();
            stack.pop_back();

            if (node->node.type == Node::Type::Block) {
                auto* b = static_cast<BlockPresentation*>(node);

                if (b->line < b->node.lines.size()) {
                    // Present next line
                    os << b->node.lines[b->line++].rpad(b->width).substr(0, b->width).str();
                } else {
                    // No more lines to present: just fill the block's width
                    os << std::string(b->width, ' ');
                }

                if (b->endl) {
                    os << std::endl;
                }

            } else if (node->node.type == Node::Type::HLayout) {
                auto* h = static_cast<HLayoutPresentation*>(node);
                // Push reversed to visit pre-order
                for (int32_t i = static_cast<int32_t>(h->children.size()) - 1; i >= 0; i--)
                    stack.push_back(&*h->children[i]);
            } else if (node->node.type == Node::Type::VLayout) {
                auto* v = static_cast<VLayoutPresentation*>(node);
                for (const auto& n : v->children) {
                    if (!n->done) {
                        stack.push_back(&*n);
                        break;
                    }
                }
            }
        }

        // 3.5) Propagate the done flag of the blocks up to all the tree.
        {
            stack.push_back(&*root);

            while (!stack.empty()) {
                NodePresentation* node = stack.back();
                stack.pop_back();

                if (node->node.type == Node::Type::Block) {
                    auto* b = static_cast<BlockPresentation*>(node);
                    b->done = b->line >= b->node.lines.size();
                    if (!b->done) {
                        // This block's render is not finished, therefore
                        // the render is not finished for all its ancestors too.
                        NodePresentation* iter = node;
                        while (iter) {
                            iter->done = false;
                            iter = iter->parent;
                        }
                    }
                } else if (node->node.type == Node::Type::HLayout || node->node.type == Node::Type::VLayout) {
                    auto* c = static_cast<ContainerPresentation*>(node);
                    // Mark as finished: eventually this will be marked as false
                    // by the propagation of the blocks through the ancestors.
                    c->done = true;
                    for (const auto& n : c->children) {
                        stack.push_back(&*n);
                    }
                }
            }
        }
    } while (!root->done);
}
} // namespace Tui
