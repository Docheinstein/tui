#include "decorators.h"
#include <iostream>

#include "block.h"
#include "hlayout.h"
#include "presenter.h"
#include "vlayout.h"

int main() {
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
     *          H1      B5
     *        /   \
     *      B1     V2
     *           /    \
     *         H2      B4
     *       /   \
     *     B2     B3
     */

    using namespace Tui;

    auto b1 {std::make_unique<Block>()};
    b1 << red("111111") << endl;
    b1 << green("111111") << endl;
    b1 << "111111" << endl;
    b1 << "111111" << endl;
    b1 << "111111" << endl;
    b1 << yellow("111111") << endl;

    // These will be expanded with spaces
    auto b2 {std::make_unique<Block>(4 /* fixed width */)};
    b2 << "222" << endl;
    b2 << magenta("222") << endl;
    b2 << "222" << endl;

    // These will be truncated (decorators will be truncated properly)
    auto b3 {std::make_unique<Block>(2 /* fixed width */)};
    b3 << "33333" << endl;
    b3 << cyan("33333") << endl;

    auto b4 {std::make_unique<Block>()};
    b4 << "4444444" << endl;
    b4 << "4444444" << endl;
    b4 << "4444444" << endl;

    auto b5 {std::make_unique<Block>()};
    b5 << "5555555555555" << endl;
    b5 << "5555555555555" << endl;
    b5 << "5555555555555" << endl;
    b5 << "5555555555555" << endl;

    auto h2 {std::make_unique<HLayout>()};
    h2->addNode(std::move(b2));
    h2->addNode(std::move(b3));

    auto v2 {std::make_unique<VLayout>()};
    v2->addNode(std::move(h2));
    v2->addNode(std::move(b4));

    auto h1 {std::make_unique<HLayout>()};
    h1->addNode(std::move(b1));
    h1->addNode(std::move(v2));

    auto v1 {std::make_unique<VLayout>()};
    v1->addNode(std::move(h1));
    v1->addNode(std::move(b5));

    Presenter p {std::cout};
    p.present(std::move(v1));

    return 0;
}
