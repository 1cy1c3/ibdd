#define CATCH_CONFIG_MAIN

#include "catch.hpp"
#include "Manager.hpp"

SCENARIO( "Working method of the unique table", "[unique-table]" )
{
    GIVEN( "Three nodes, one key and slots in the unique table" )
    {
        UTable<TableKey, DDNode*> uTable;
        uTable.load(521);
        BDDNode a(1);
        BDDNode b(2);
        BDDNode c(3);
        REQUIRE(a.getDDNode() != 0);
        REQUIRE(b.getDDNode() != 0);
        REQUIRE(c.getDDNode() != 0);
        TableKey key( a.getDDNode(), b.getDDNode(), c.getDDNode() );
        REQUIRE(uTable.getSize() == 521);
        DDNode* ddNode = new DDNode( a.getDDNode(), b.getDDNode(), c.getDDNode() );
        WHEN("a node is added") {
            uTable.add(key, ddNode);
            THEN("it should also be found") {
                REQUIRE(uTable.find(key, ddNode) == true);
            }
        }
        WHEN("a garbage collection is performed") {
            uTable.clear();
            THEN("the unique table should be empty") {
                REQUIRE(uTable.empty() == true);
            }
        }
    }
}
SCENARIO( "Working method of the computed table", "[computed-table]" )
{
    GIVEN( "Three nodes, one key and slots in the computed table" )
    {
        CTable<TableKey, DDNode*> cTable;
        cTable.load(521);
        BDDNode a(1);
        BDDNode b(2);
        BDDNode c(3);
        REQUIRE(a.getDDNode() != 0);
        REQUIRE(b.getDDNode() != 0);
        REQUIRE(c.getDDNode() != 0);
        TableKey key( a.getDDNode(), b.getDDNode(), c.getDDNode() );
        REQUIRE(cTable.getSize() == 521);
        DDNode* ddNode = new DDNode( a.getDDNode(), b.getDDNode(), c.getDDNode() );
        WHEN("a node is added") {
            cTable.insert(key, ddNode);
            THEN("it should also be found") {
                REQUIRE(cTable.hasNext(key, ddNode) == true);
            }
        }
        WHEN("a garbage collection is performed") {
            cTable.clear();
            THEN("the computed table should be empty") {
                REQUIRE(cTable.empty() == true);
            }
        }
    }
}
SCENARIO( "Application of operations to BDDs", "[operator]" )
{
    GIVEN( "Two BDDs und slots in the unique as well as computed table" )
    {
        Manager manager(2, 521, 521);
        BDDNode a( manager.createVariable(1) );
        BDDNode b( manager.createVariable(2) );
        REQUIRE(a.countNodes() == 2);
        REQUIRE(b.countNodes() == 2);
        WHEN("the conjunction is applied") {
            BDDNode f = a * b;
            THEN("three nodes should exist") {
                REQUIRE(f.countNodes() == 3);
            }
        }
        WHEN("the disjunction is applied") {
            BDDNode f = a + b;
            THEN("three nodes should exist") {
                REQUIRE(f.countNodes() == 3);
            }
        }
        WHEN("the adjunction is applied") {
            BDDNode f = a ^ b;
            THEN("three nodes should exist") {
                REQUIRE(f.countNodes() == 3);
            }
        }
        WHEN("the \"More than\" operator is applied") {
            BDDNode f = a > b;
            THEN("three nodes should exist") {
                REQUIRE(f.countNodes() == 3);
            }
        }
        WHEN("the \"Less than\" operator is applied") {
            BDDNode f = a < b;
            THEN("three nodes should exist") {
                REQUIRE(f.countNodes() == 3);
            }
        }
        WHEN("the XNOR operator is applied") {
            BDDNode f = a % b;
            THEN("three nodes should exist") {
                REQUIRE(f.countNodes() == 3);
            }
        }
        WHEN("the NAND operator is applied") {
            BDDNode f = a & b;
            THEN("three nodes should exist") {
                REQUIRE(f.countNodes() == 3);
            }
        }
        WHEN("the NOR operator is applied") {
            BDDNode f = a | b;
            THEN("three nodes should exist") {
                REQUIRE(f.countNodes() == 3);
            }
        }
        WHEN("the NOT operator is applied") {
            BDDNode f = !a;
            THEN("two nodes should exist") {
                REQUIRE(f.countNodes() == 2);
            }
        }
        WHEN("the high child is computed") {
            BDDNode f = a.getCofactor( 1, BDDNode::getHighFactor() );
            THEN("one node should exist") {
                REQUIRE(f.countNodes() == 1);
            }
        }
        WHEN("the low child is computed") {
            BDDNode f = a.getCofactor( 1, BDDNode::getLowFactor() );
            THEN("one node should exist") {
                REQUIRE(f.countNodes() == 1);
            }
        }
        WHEN("the variable 1 is quantified existentially") {
            BDDNode f = a.exist(1);
            THEN("one node should exist") {
                REQUIRE(f.countNodes() == 1);
            }
        }
    }
}
SCENARIO( "BDDs with complement edges", "[complementEdges]" )
{
    GIVEN( "Two BDDs und slots in the unique as well as computed table" )
    {
        Manager manager(4, 521, 521);
        BDDNode a( manager.createVariable(1) );
        BDDNode b( manager.createVariable(2) );
        REQUIRE(a.countNodes() == 2);
        REQUIRE(b.countNodes() == 2);
        WHEN("the function f=!(a*b) exists") {
            BDDNode f = !(a * b);
            THEN("the root node should have a complement edge") {
                REQUIRE(f.isComplementEdge() == true);
            }
        }
        WHEN("the function f=a*b exists") {
            BDDNode f = a * b;
            THEN("the root node should not have a complement edge") {
                REQUIRE(f.isComplementEdge() == false);
            }
        }
    }
}
SCENARIO( "Nodes and their references", "[referenceCounter]" )
{
    GIVEN( "Four BDDs und slots in the unique as well as computed table" )
    {
        Manager manager(4, 521, 521);
        BDDNode a( manager.createVariable(1) );
        BDDNode b( manager.createVariable(2) );
        BDDNode c( manager.createVariable(3) );
        BDDNode d( manager.createVariable(4) );
        REQUIRE(a.countNodes() == 2);
        REQUIRE(b.countNodes() == 2);
        REQUIRE(c.countNodes() == 2);
        REQUIRE(d.countNodes() == 2);
        WHEN("a node is created for a variable") {
            THEN("the cofactor should be a leaf") {
                REQUIRE(a.getCofactor(1, BDDNode::getHighFactor()).isLeaf() == true);
            }
        }
        WHEN("a combination of BDDs exists") {
            BDDNode g = (a * b) ^ (!c | d);
            BDDNode h = g.getCofactor( 1, BDDNode::getHighFactor() );
            BDDNode f = (g ^ h).exist(3);
            THEN("the root node should have been reused once") {
                REQUIRE(f.getDDNodeWithEdge()->getID() == 2);
            }
        }
    }
}
