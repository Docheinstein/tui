#include "presenter.h"
#include "block.h"
#include "decorators.h"
#include "divider.h"
#include "hlayout.h"
#include "vlayout.h"
#include <iostream>

namespace Tui {
Presenter::Presenter(std::ostream& os) :
    os(os) {
}

struct PNode {
    struct Type {
        using PNodeType = uint8_t;
        static constexpr PNodeType Block = 1 << 0;
        static constexpr PNodeType HLayout = 1 << 1;
        static constexpr PNodeType VLayout = 1 << 2;
        static constexpr PNodeType HDivider = 1 << 3;
        static constexpr PNodeType VDivider = 1 << 4;

        static constexpr PNodeType Divider = HDivider | VDivider;
        static constexpr PNodeType Content = Block | Divider;
        static constexpr PNodeType Container = HLayout | VLayout;
    };

    explicit PNode(Type::PNodeType type, Node& node, PNode* parent) :
        type(type),
        node(node),
        parent(parent) {
    }

    Type::PNodeType type;
    Node& node;
    PNode* parent {};

    std::optional<uint32_t> width {};
    std::optional<uint32_t> height {};

    bool done {};
};

struct PContent : PNode {
    PContent(Type::PNodeType type, Node& node, PNode* parent) :
        PNode(type, node, parent) {
    }

    bool endl {};
    uint32_t line {};
};

struct PBlock : PContent {
    PBlock(Block& node, PNode* parent) :
        PContent(Type::Block, node, parent),
        node(node) {
    }

    Block& node;
};

struct PDivider : PContent {
    PDivider(Type::PNodeType type, Divider& node, PNode* parent) :
        PContent(type, node, parent),
        node(node) {
    }

    Divider& node;
};

struct PHDivider : PDivider {
    PHDivider(Divider& node, PNode* parent) :
        PDivider(Type::HDivider, node, parent) {
    }
};

struct PVDivider : PDivider {
    PVDivider(Divider& node, PNode* parent) :
        PDivider(Type::VDivider, node, parent) {
    }
};

struct PContainer : PNode {
    PContainer(Type::PNodeType type, Container& node, PNode* parent) :
        PNode(type, node, parent),
        node(node) {
    }

    Container& node;
    std::vector<std::unique_ptr<PNode>> children;
};

struct PHLayout : PContainer {
    PHLayout(HLayout& node, PNode* parent) :
        PContainer(Type::HLayout, node, parent),
        node(node) {
    }

    HLayout& node;
};

struct PVLayout : PContainer {
    PVLayout(VLayout& node, PNode* parent) :
        PContainer(Type::VLayout, node, parent),
        node(node) {
    }

    VLayout& node;
};

void Presenter::present(std::unique_ptr<Node>&& rootNode) {
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

    std::unique_ptr<PNode> root;

    // 1) First visit of the tree.
    //    - Wrap each node with a presentation node (wrapper that adds helpers).
    {
        const auto makePNode = [](Node& node, PNode* parent) -> std::unique_ptr<PNode> {
            if (node.type == Node::Type::Block) {
                return std::make_unique<PBlock>(static_cast<Block&>(node), parent);
            } else if (node.type == Node::Type::Divider) {
                if (parent->node.type == Node::Type::HLayout)
                    return std::make_unique<PHDivider>(static_cast<Divider&>(node), parent);
                if (parent->node.type == Node::Type::VLayout)
                    return std::make_unique<PVDivider>(static_cast<Divider&>(node), parent);
            } else if (node.type == Node::Type::HLayout) {
                return std::make_unique<PHLayout>(static_cast<HLayout&>(node), parent);
            } else if (node.type == Node::Type::VLayout) {
                return std::make_unique<PVLayout>(static_cast<VLayout&>(node), parent);
            }
            return nullptr;
        };

        root = makePNode(*rootNode, nullptr);

        std::vector<PNode*> stack;
        stack.push_back(&*root);

        while (!stack.empty()) {
            PNode* node = stack.back();
            stack.pop_back();

            if (node->node.type == Node::Type::HLayout || node->node.type == Node::Type::VLayout) {
                // Wrap each child into a presentation node
                auto* c = static_cast<PContainer*>(node);
                c->children.resize(c->node.children.size());
                for (uint32_t i = 0; i < c->children.size(); i++) {
                    c->children[i] = makePNode(*c->node.children[i], node);
                    stack.push_back(&*c->children[i]);
                }
            }
        }
    }

    struct PostOrderPNodeStackEntry {
        PNode* node;
        uint32_t index {};
    };

    // 2) Compute dimensions of nodes with fixed size
    //    and propagate information up to all the tree
    //    (e.g. to containers)
    {
        std::vector<PostOrderPNodeStackEntry> stack {{&*root}};

        while (!stack.empty()) {
            PostOrderPNodeStackEntry& entry = stack.back();
            PNode* node = entry.node;

            bool visit {true};

            if (node->type & PNode::Type::Block) {
                auto* b = static_cast<PBlock*>(node);

                // Eventually strip out empty new lines
                for (int32_t i = static_cast<int32_t>(b->node.lines.size()) - 1; i >= 0; i--) {
                    if (b->node.lines[i].size() == 0)
                        b->node.lines.pop_back();
                }

                // Compute block's dimensions
                if (b->node.width) {
                    // Fixed width
                    b->width = *b->node.width;
                } else {
                    // Variable width
                    for (const auto& l : b->node.lines) {
                        b->width = std::max(b->width.value_or(0), l.size().value);
                    }
                }
                b->height = b->node.lines.size();
            } else if (node->type & PNode::Type::HDivider) {
                auto* d = static_cast<PDivider*>(node);
                d->width = d->node.text.size();
            } else if (node->type & PNode::Type::VDivider) {
                auto* d = static_cast<PDivider*>(node);
                d->height = 1;
            } else if (node->type & PNode::Type::Container) {
                const auto* c = static_cast<const PContainer*>(node);
                if (entry.index < c->children.size()) {
                    // Still children to visit
                    uint32_t idx = entry.index++;
                    stack.push_back({&*c->children[idx]});
                    visit = false;
                } else {
                    // Compute container's dimensions
                    if (node->type & PNode::Type::HLayout) {
                        for (const auto& n : c->children) {
                            node->width = node->width.value_or(0) + n->width.value_or(0);
                            node->height = std::max(node->height.value_or(0), n->height.value_or(0));
                        }
                    } else if (node->type & PNode::Type::VLayout) {
                        for (const auto& n : c->children) {
                            node->width = std::max(node->width.value_or(0), n->width.value_or(0));
                            node->height = node->height.value_or(0) + n->height.value_or(0);
                        }
                    }
                }
            }

            if (visit) {
                stack.pop_back();
            }
        }
    }

    // 3) Propagate dimensions down to automatically sized nodes (e.g. dividers).
    {
        std::vector<PNode*> stack {&*root};

        while (!stack.empty()) {
            PNode* node = stack.back();
            stack.pop_back();

            if (!node->width)
                node->width = node->parent ? node->parent->width.value_or(0) : 0;
            if (!node->height)
                node->height = node->parent ? node->parent->height.value_or(0) : 0;

            if (node->type & PNode::Type::Container) {
                const auto* c = static_cast<const PContainer*>(node);
                for (const auto& n : c->children) {
                    stack.push_back(&*n);
                }
            }
        }
    }

    // 4) Find ending blocks (the ones at the right of the entire content)
    //    An ending block is defined by the following rule:
    //    A block is an ending block unless it is not part of the rightmost
    //    branch of any Horizontal Layout ancestors it has.
    {
        std::vector<PNode*> stack {&*root};

        while (!stack.empty()) {
            PNode* node = stack.back();
            stack.pop_back();

            if (node->type & PNode::Type::Content) {
                static_cast<PContent*>(node)->endl = true;
            } else if (node->node.type == Node::Type::HLayout) {
                const auto* h = static_cast<const PHLayout*>(node);
                if (!h->children.empty()) {
                    // Only the blocks of the right most branch are candidates.
                    stack.push_back(&*h->children.back());
                }
            } else if (node->node.type == Node::Type::VLayout) {
                const auto* v = static_cast<const PVLayout*>(node);
                for (const auto& n : v->children) {
                    stack.push_back(&*n);
                }
            }
        }
    }

    // 5) Presentation.
    //    The logic is the following:
    //
    //    [Container]
    //
    //    - Horizontal Layout:
    //          Processed in parallel.
    //          When it is visited all its children are visited too.
    //    - Vertical Layout
    //          Processed sequentially.
    //          When it is visited only the first child for which the render
    //          is not finished is visited.
    //
    //    [Content]
    //
    //    - Block/Dividers
    //          When it is visited it pushes the next line to the output stream.
    //          The lines are truncated/expanded to exactly fill the block's width.
    //          If there are no more lines to render, it pushes empty lines
    //          to fill the block's width.
    do {
        // A) Presentation.
        {
            std::vector<PNode*> stack {&*root};

            while (!stack.empty()) {
                PNode* node = stack.back();
                stack.pop_back();

                if (node->type & PNode::Type::Block) {
                    auto* b = static_cast<PBlock*>(node);

                    if (b->line < b->node.lines.size()) {
                        // Present next line
                        const Text& l = b->node.lines[b->line];
                        Text t {};
                        uint32_t w = *b->width;
                        if (t.size() < w)
                            t = l.rpad(Text::Length {w});
                        else if (t.size() > w)
                            t = l.substr(Text::RawIndex {0}, Text::Length {w});
                        else
                            t = l;
                        os << t.str();

                        // Always push the reset attribute in case substr truncated it
                        os << reset().str();
                    } else {
                        // No more lines to present: just fill the block's width
                        os << std::string(*b->width, ' ');
                    }

                    if (b->endl)
                        os << std::endl;

                    b->line++;
                } else if (node->type & PNode::Type::Divider) {
                    auto* d = static_cast<PDivider*>(node);

                    for (uint32_t i = 0; i < *node->width; i += d->node.text.size()) {
                        os << d->node.text.str();
                    }

                    if (d->endl)
                        os << std::endl;

                    d->line++;
                } else if (node->node.type == Node::Type::HLayout) {
                    auto* h = static_cast<PHLayout*>(node);
                    // Push reversed to visit pre-order
                    for (int32_t i = static_cast<int32_t>(h->children.size()) - 1; i >= 0; i--)
                        stack.push_back(&*h->children[i]);
                } else if (node->node.type == Node::Type::VLayout) {
                    auto* v = static_cast<PVLayout*>(node);
                    for (const auto& n : v->children) {
                        if (!n->done) {
                            stack.push_back(&*n);
                            break;
                        }
                    }
                }
            }
        }

        // B) Propagate the done flag of the blocks up to all the tree.
        {
            std::vector<PostOrderPNodeStackEntry> stack {{&*root}};

            while (!stack.empty()) {
                PostOrderPNodeStackEntry& entry = stack.back();
                PNode* node = entry.node;

                bool visit {true};

                if (node->type & PNode::Type::Content) {
                    auto* c = static_cast<PContent*>(node);
                    c->done = c->line >= *c->height;
                } else if (node->type & PNode::Type::Container) {
                    const auto* c = static_cast<const PContainer*>(node);
                    if (entry.index < c->children.size()) {
                        // Still children to visit
                        uint32_t idx = entry.index++;
                        stack.push_back({&*c->children[idx]});
                        visit = false;
                    } else {
                        // Mark this container as done if all the children have done
                        node->done = true;
                        for (const auto& n : c->children) {
                            node->done &= n->done;
                        }
                    }
                }

                if (visit) {
                    stack.pop_back();
                }
            }
        }
    } while (!root->done);
}
} // namespace Tui
