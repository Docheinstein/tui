#include "tui/decorators.h"
#include <iostream>

#include "tui/block.h"
#include "tui/divider.h"
#include "tui/factory.h"
#include "tui/hlayout.h"
#include "tui/presenter.h"
#include "tui/vlayout.h"

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

#if 0
    auto b1 {make_block()};
    b1 << red("111111\n11") << endl;
    b1 << green("111111") << endl;
    b1 << "111111" << endl;
    b1 << "111111" << endl;
    b1 << "111111" << endl;
    b1 << yellow("111111") << endl;

    // These will be expanded with spaces
    auto b2 {make_block(4 /* fixed width */)};
    b2 << "222" << endl;
    b2 << magenta("222") << endl;
    b2 << "222" << endl;

    // These will be truncated (decorators will be truncated properly)
    auto b3 {make_block(2 /* fixed width */)};
    b3 << "33333" << endl;
    b3 << cyan("33333") << endl;

    auto b4 {make_block()};
    b4 << "4444444" << endl;
    b4 << "4444444" << endl;
    b4 << "4444444" << endl;

    auto b5 {make_block()};
    b5 << "5555555555555" << endl;
    b5 << "5555555555555" << endl;
    b5 << "5555555555555" << endl;
    b5 << "5555555555555" << endl;

    auto h2 {make_horizontal_layout()};
    h2->addNode(std::move(b2));
    h2->addNode(make_divider("|"));
    h2->addNode(std::move(b3));

    auto v2 {make_vertical_layout()};
    v2->addNode(std::move(h2));
    v2->addNode(std::move(b4));

    auto h1 {make_horizontal_layout()};
    h1->addNode(std::move(b1));
    h1->addNode(std::move(v2));

    auto v1 {make_vertical_layout()};
    v1->addNode(std::move(h1));
    v1->addNode(make_divider("-"));
    v1->addNode(std::move(b5));

    Presenter p {std::cout};
    p.present(*v1);
#endif
    using namespace Tui;

    auto b1 {make_block()};
    b1 << "111" << endl;
    b1 << "111" << endl;

    auto b2 {make_block()};
    b2 << "22" << endl;
    b2 << "22" << endl;

    auto b3 {make_block()};
    b3 << red("333333") << endl;

    auto h1 {make_horizontal_layout()};
    h1->addNode(std::move(b1));
    h1->addNode(make_divider("|"));
    h1->addNode(std::move(b2));

    auto v1 {make_vertical_layout()};
    v1->addNode(std::move(h1));
    v1->addNode(make_divider("-"));
    v1->addNode(std::move(b3));

    Presenter p {std::cout};
    p.present(*v1);

    return 0;
}
