#include "../../spa/src/Utility/CFG/control_flow_graph.h"
#include "../../spa/src/Utility/CFG/cfg_token.h"
#include "SP/tokenizer.h"

#include <fstream>
#include <unordered_map>
#include "catch.hpp"

using namespace std;

TEST_CASE("Read/print statements") {

  SECTION("1_test4") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kRead, 1),
      CFGToken(CFGTokenType::kRead, 2),
      CFGToken(CFGTokenType::kPrint, 3),
      CFGToken(CFGTokenType::kPrint, 4),
      CFGToken(CFGTokenType::kRead, 5),
      CFGToken(CFGTokenType::kPrint, 6),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);

    //start
    shared_ptr<cfg::GraphNode> start = make_shared<cfg::GraphNode>(cfg::NodeType::START, 0);

    //line 1 - 6
    shared_ptr<cfg::GraphNode> stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 1);
    stmt->start_ = 1;
    stmt->end_ = 6;

    unordered_map<int, EntityIdentifier> stmt_type = {
      { 1, EntityIdentifier::kRead },
      { 2, EntityIdentifier::kRead },
      { 3, EntityIdentifier::kPrint },
      { 4, EntityIdentifier::kPrint },
      { 5, EntityIdentifier::kRead },
      { 6, EntityIdentifier::kPrint },
    };
    stmt->stmt_type_ = move(stmt_type);

    //end 
    shared_ptr<cfg::GraphNode> end_node = make_shared<cfg::GraphNode>(cfg::NodeType::END, 0);

    //connect them
    start->next_node_ = stmt;
    stmt->next_node_ = end_node;

    cfg::CFG cfg_ans(start);

    REQUIRE(cfg.equal(cfg_ans));
  }
}

TEST_CASE("Read/print/assign statements") {

  SECTION("2_test4") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kRead, 1),
      CFGToken(CFGTokenType::kAssign, 2),
      CFGToken(CFGTokenType::kPrint, 3),
      CFGToken(CFGTokenType::kRead, 4),
      CFGToken(CFGTokenType::kAssign, 5),
      CFGToken(CFGTokenType::kAssign, 6),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    //start
    shared_ptr<cfg::GraphNode> start = make_shared<cfg::GraphNode>(cfg::NodeType::START, 0);

    //line 1 - 6
    shared_ptr<cfg::GraphNode> stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 1);
    stmt->start_ = 1;
    stmt->end_ = 6;

    unordered_map<int, EntityIdentifier> stmt_type = {
      { 1, EntityIdentifier::kRead },
      { 2, EntityIdentifier::kAssign },
      { 3, EntityIdentifier::kPrint },
      { 4, EntityIdentifier::kRead },
      { 5, EntityIdentifier::kAssign },
      { 6, EntityIdentifier::kAssign },
    };
    stmt->stmt_type_ = move(stmt_type);

    //end 
    shared_ptr<cfg::GraphNode> end_node = make_shared<cfg::GraphNode>(cfg::NodeType::END, 0);

    //connect them
    start->next_node_ = stmt;
    stmt->next_node_ = end_node;

    cfg::CFG cfg_ans(start);

    REQUIRE(cfg.equal(cfg_ans));
  }
}

TEST_CASE("Read/print/assign/if/while statments (1 level nesting)") {

  SECTION("3_test1") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kAssign, 1),
      CFGToken(CFGTokenType::kIf, 2),
      CFGToken(CFGTokenType::kPrint, 3),
      CFGToken(CFGTokenType::kRead, 4),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kAssign, 5),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    //start
    shared_ptr<cfg::GraphNode> start = make_shared<cfg::GraphNode>(cfg::NodeType::START, 0);

    //line 1 
    shared_ptr<cfg::GraphNode> assign1_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 1);
    assign1_stmt->stmt_type_.insert(make_pair(1, EntityIdentifier::kAssign));

    //line 2
    shared_ptr<cfg::GraphNode> if_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IF, 2);
    if_stmt->stmt_type_.insert(make_pair(2, EntityIdentifier::kIf));

    //line 3 & 4
    shared_ptr<cfg::GraphNode> read_print_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 3);
    read_print_stmt->end_ = 4;
    read_print_stmt->stmt_type_.insert(make_pair(3, EntityIdentifier::kPrint));
    read_print_stmt->stmt_type_.insert(make_pair(4, EntityIdentifier::kRead));

    //then end
    shared_ptr<cfg::GraphNode> then_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::THENEND, 0);

    //line 5 
    shared_ptr<cfg::GraphNode> assign5_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 5);
    assign5_stmt->stmt_type_.insert(make_pair(5, EntityIdentifier::kAssign));

    //if end
    shared_ptr<cfg::GraphNode> if_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IFEND, 0);

    //end 
    shared_ptr<cfg::GraphNode> end_node = make_shared<cfg::GraphNode>(cfg::NodeType::END, 0);

    //connect them
    start->next_node_ = assign1_stmt;
    assign1_stmt->next_node_ = if_stmt;
    if_stmt->next_node_ = read_print_stmt;
    if_stmt->alternative_node_ = assign5_stmt;
    read_print_stmt->next_node_ = then_end_stmt;
    then_end_stmt->next_node_ = if_end_stmt;
    assign5_stmt->next_node_ = if_end_stmt;
    if_end_stmt->next_node_ = end_node;

    cfg::CFG cfg_ans(start);

    REQUIRE(cfg.equal(cfg_ans));
  }

  SECTION("3_test2") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kRead, 1),
      CFGToken(CFGTokenType::kWhile, 2),
      CFGToken(CFGTokenType::kAssign, 3),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kPrint, 4),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    //start
    shared_ptr<cfg::GraphNode> start = make_shared<cfg::GraphNode>(cfg::NodeType::START, 0);

    //line 1 
    shared_ptr<cfg::GraphNode> read_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 1);
    read_stmt->stmt_type_.insert(make_pair(1, EntityIdentifier::kRead));

    //line 2
    shared_ptr<cfg::GraphNode> while_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILE, 2);
    while_stmt->stmt_type_.insert(make_pair(2, EntityIdentifier::kWhile));

    //line 3
    shared_ptr<cfg::GraphNode> assign_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 3);
    assign_stmt->stmt_type_.insert(make_pair(3, EntityIdentifier::kAssign));

    //while end
    shared_ptr<cfg::GraphNode> while_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILEEND, 0);
     
    //line 4
    shared_ptr<cfg::GraphNode> print_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 4);
    print_stmt->stmt_type_.insert(make_pair(4, EntityIdentifier::kPrint));

    //end 
    shared_ptr<cfg::GraphNode> end_node = make_shared<cfg::GraphNode>(cfg::NodeType::END, 0);

    //connect them
    start->next_node_ = read_stmt;
    read_stmt->next_node_ = while_stmt;
    while_stmt->next_node_ = assign_stmt;
    while_stmt->alternative_node_ = print_stmt;
    assign_stmt->next_node_ = while_end_stmt;
    while_end_stmt->next_node_ = while_stmt;
    print_stmt->next_node_ = end_node;

    cfg::CFG cfg_ans(start);

    REQUIRE(cfg.equal(cfg_ans));
  }

  SECTION("3_test3") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kIf, 1),
      CFGToken(CFGTokenType::kRead, 2),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kRead, 3),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kWhile, 4),
      CFGToken(CFGTokenType::kPrint, 5),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kAssign, 6),
      CFGToken(CFGTokenType::kPrint, 7),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    //start
    shared_ptr<cfg::GraphNode> start = make_shared<cfg::GraphNode>(cfg::NodeType::START, 0);

    //line 1
    shared_ptr<cfg::GraphNode> if_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IF, 1);
    if_stmt->stmt_type_.insert(make_pair(1, EntityIdentifier::kIf));

    //line 2
    shared_ptr<cfg::GraphNode> read2_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 2);
    read2_stmt->stmt_type_.insert(make_pair(2, EntityIdentifier::kRead));

    //then end
    shared_ptr<cfg::GraphNode> then_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::THENEND, 0);

    //line 3
    shared_ptr<cfg::GraphNode> read3_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 3);
    read3_stmt->stmt_type_.insert(make_pair(3, EntityIdentifier::kRead));

    //if end
    shared_ptr<cfg::GraphNode> if_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IFEND, 0);

    //line 4
    shared_ptr<cfg::GraphNode> while_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILE, 4);
    while_stmt->stmt_type_.insert(make_pair(4, EntityIdentifier::kWhile));

    //line 5
    shared_ptr<cfg::GraphNode> print_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 5);
    print_stmt->stmt_type_.insert(make_pair(5, EntityIdentifier::kPrint));

    //while end
    shared_ptr<cfg::GraphNode> while_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILEEND, 0);

    //line 6 & 7
    shared_ptr<cfg::GraphNode> assign_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 6);
    assign_stmt->end_ = 7;
    assign_stmt->stmt_type_.insert(make_pair(6, EntityIdentifier::kAssign));
    assign_stmt->stmt_type_.insert(make_pair(7, EntityIdentifier::kPrint));

    //end 
    shared_ptr<cfg::GraphNode> end_node = make_shared<cfg::GraphNode>(cfg::NodeType::END, 0);

    //connect them
    start->next_node_ = if_stmt;
    if_stmt->next_node_ = read2_stmt;
    if_stmt->alternative_node_ = read3_stmt;
    read2_stmt->next_node_ = then_end_stmt;
    then_end_stmt->next_node_ = if_end_stmt;
    read3_stmt->next_node_ = if_end_stmt;
    if_end_stmt->next_node_ = while_stmt;

    while_stmt->next_node_ = print_stmt;
    while_stmt->alternative_node_ = assign_stmt;
    print_stmt->next_node_ = while_end_stmt;
    while_end_stmt->next_node_ = while_stmt;
    assign_stmt->next_node_ = end_node;

    cfg::CFG cfg_ans(start);

    REQUIRE(cfg.equal(cfg_ans));
  }

  SECTION("3_test4") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kRead, 1),
      CFGToken(CFGTokenType::kAssign, 2),
      CFGToken(CFGTokenType::kPrint, 3),
      CFGToken(CFGTokenType::kIf, 4),
      CFGToken(CFGTokenType::kAssign, 5),
      CFGToken(CFGTokenType::kAssign, 6),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kPrint, 7),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kRead, 8),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    //start
    shared_ptr<cfg::GraphNode> start = make_shared<cfg::GraphNode>(cfg::NodeType::START, 0);
    
    //line 1 - 3
    shared_ptr<cfg::GraphNode> read_assign_print_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 1);
    read_assign_print_stmt->end_ = 3;
    read_assign_print_stmt->stmt_type_.insert(make_pair(1, EntityIdentifier::kRead));
    read_assign_print_stmt->stmt_type_.insert(make_pair(2, EntityIdentifier::kAssign));
    read_assign_print_stmt->stmt_type_.insert(make_pair(3, EntityIdentifier::kPrint));

    //line 4
    shared_ptr<cfg::GraphNode> if_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IF, 4);
    if_stmt->stmt_type_.insert(make_pair(4, EntityIdentifier::kIf));

    //line 5 - 6
    shared_ptr<cfg::GraphNode> assigns_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 5);
    assigns_stmt->end_ = 6;
    assigns_stmt->stmt_type_.insert(make_pair(5, EntityIdentifier::kAssign));
    assigns_stmt->stmt_type_.insert(make_pair(6, EntityIdentifier::kAssign));

    //then end
    shared_ptr<cfg::GraphNode> then_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::THENEND, 0);

    //line 7
    shared_ptr<cfg::GraphNode> print_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 7);
    print_stmt->stmt_type_.insert(make_pair(7, EntityIdentifier::kPrint));

    //if end
    shared_ptr<cfg::GraphNode> if_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IFEND, 0);
    
    //line 8
    shared_ptr<cfg::GraphNode> read8_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 8);
    read8_stmt->stmt_type_.insert(make_pair(8, EntityIdentifier::kRead));

    //end 
    shared_ptr<cfg::GraphNode> end_node = make_shared<cfg::GraphNode>(cfg::NodeType::END, 0);

    //connect them
    start->next_node_ = read_assign_print_stmt;
    read_assign_print_stmt->next_node_ = if_stmt;

    if_stmt->next_node_ = assigns_stmt;
    if_stmt->alternative_node_ = print_stmt;
    assigns_stmt->next_node_ = then_end_stmt;
    then_end_stmt->next_node_ = if_end_stmt;
    print_stmt->next_node_ = if_end_stmt;
    if_end_stmt->next_node_ = read8_stmt;

    read8_stmt->next_node_ = end_node;

    cfg::CFG cfg_ans(start);

    REQUIRE(cfg.equal(cfg_ans));
  }
}

TEST_CASE("Read/print/assign/if/while statments (2 level nesting)") {

  SECTION("4_test1") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kAssign, 1),
      CFGToken(CFGTokenType::kWhile, 2),
      CFGToken(CFGTokenType::kIf, 3),
      CFGToken(CFGTokenType::kPrint, 4),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kRead, 5),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    //start
    shared_ptr<cfg::GraphNode> start = make_shared<cfg::GraphNode>(cfg::NodeType::START, 0);

    //line 1
    shared_ptr<cfg::GraphNode> assign_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 1);
    assign_stmt->stmt_type_.insert(make_pair(1, EntityIdentifier::kAssign));

    //line 2
    shared_ptr<cfg::GraphNode> while_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILE, 2);
    while_stmt->stmt_type_.insert(make_pair(2, EntityIdentifier::kWhile));

    //line 3
    shared_ptr<cfg::GraphNode> if_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IF, 3);
    if_stmt->stmt_type_.insert(make_pair(3, EntityIdentifier::kIf));

    //line 4
    shared_ptr<cfg::GraphNode> print_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 4);
    print_stmt->stmt_type_.insert(make_pair(4, EntityIdentifier::kPrint));

    //then end
    shared_ptr<cfg::GraphNode> then_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::THENEND, 0);

    //line 5
    shared_ptr<cfg::GraphNode> read_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 5);
    read_stmt->stmt_type_.insert(make_pair(5, EntityIdentifier::kRead));

    //if end
    shared_ptr<cfg::GraphNode> if_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IFEND, 0);

    //while end
    shared_ptr<cfg::GraphNode> while_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILEEND, 0);

    //end 
    shared_ptr<cfg::GraphNode> end_node = make_shared<cfg::GraphNode>(cfg::NodeType::END, 0);

    //connect them
    start->next_node_ = assign_stmt;
    assign_stmt->next_node_ = while_stmt;

    while_stmt->next_node_ = if_stmt;

    if_stmt->next_node_ = print_stmt;
    if_stmt->alternative_node_ = read_stmt;
    print_stmt->next_node_ = then_end_stmt;
    then_end_stmt->next_node_ = if_end_stmt;
    read_stmt->next_node_ = if_end_stmt;
    if_end_stmt->next_node_ = while_end_stmt;

    while_end_stmt->next_node_ = while_stmt;
    while_stmt->alternative_node_ = end_node;

    cfg::CFG cfg_ans(start);

    REQUIRE(cfg.equal(cfg_ans));
  }

  SECTION("4_test2") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kRead, 1),
      CFGToken(CFGTokenType::kIf, 2),
      CFGToken(CFGTokenType::kAssign, 3),
      CFGToken(CFGTokenType::kWhile, 4),
      CFGToken(CFGTokenType::kAssign, 5),
      CFGToken(CFGTokenType::kPrint, 6),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kPrint, 7),
      CFGToken(CFGTokenType::kAssign, 8),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kPrint, 9),
      CFGToken(CFGTokenType::kAssign, 10),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    //start
    shared_ptr<cfg::GraphNode> start = make_shared<cfg::GraphNode>(cfg::NodeType::START, 0);

    //line 1
    shared_ptr<cfg::GraphNode> read_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 1);
    read_stmt->stmt_type_.insert(make_pair(1, EntityIdentifier::kRead));

    //line 2
    shared_ptr<cfg::GraphNode> if_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IF, 2);
    if_stmt->stmt_type_.insert(make_pair(2, EntityIdentifier::kIf));

    //line 3
    shared_ptr<cfg::GraphNode> assign3_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 3);
    assign3_stmt->stmt_type_.insert(make_pair(3, EntityIdentifier::kAssign));

    //line 4
    shared_ptr<cfg::GraphNode> while_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILE, 4);
    while_stmt->stmt_type_.insert(make_pair(4, EntityIdentifier::kWhile));

    //line 5 - 6
    shared_ptr<cfg::GraphNode> assign_print_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 5);
    assign_print_stmt->end_ = 6;
    assign_print_stmt->stmt_type_.insert(make_pair(5, EntityIdentifier::kAssign));
    assign_print_stmt->stmt_type_.insert(make_pair(6, EntityIdentifier::kPrint));

    //while end
    shared_ptr<cfg::GraphNode> while_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILEEND, 0);

    //then end
    shared_ptr<cfg::GraphNode> then_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::THENEND, 0);

    //line 7 - 8
    shared_ptr<cfg::GraphNode> print_assign_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 7);
    print_assign_stmt->end_ = 8;
    print_assign_stmt->stmt_type_.insert(make_pair(7, EntityIdentifier::kPrint));
    print_assign_stmt->stmt_type_.insert(make_pair(8, EntityIdentifier::kAssign));

    //if end
    shared_ptr<cfg::GraphNode> if_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IFEND, 0);

    //line 9 - 10
    shared_ptr<cfg::GraphNode> print_assign2_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 9);
    print_assign2_stmt->end_ = 10;
    print_assign2_stmt->stmt_type_.insert(make_pair(9, EntityIdentifier::kPrint));
    print_assign2_stmt->stmt_type_.insert(make_pair(10, EntityIdentifier::kAssign));

    //end 
    shared_ptr<cfg::GraphNode> end_node = make_shared<cfg::GraphNode>(cfg::NodeType::END, 0);

    //connect them
    start->next_node_ = read_stmt;
    read_stmt->next_node_ = if_stmt;

    if_stmt->next_node_ = assign3_stmt;
    assign3_stmt->next_node_ = while_stmt;
    while_stmt->next_node_ = assign_print_stmt;
    while_stmt->alternative_node_ = then_end_stmt;
    assign_print_stmt->next_node_ = while_end_stmt;
    while_end_stmt->next_node_ = while_stmt;
    
    if_stmt->alternative_node_ = print_assign_stmt;
    then_end_stmt->next_node_ = if_end_stmt;
    print_assign_stmt->next_node_ = if_end_stmt;
    
    if_end_stmt->next_node_ = print_assign2_stmt;
    print_assign2_stmt->next_node_ = end_node;
   
    cfg::CFG cfg_ans(start);

    REQUIRE(cfg.equal(cfg_ans));
  }
}

TEST_CASE("Read/print/assign/if/while statments (3 level nesting)") {

  SECTION("5_test1") {
    vector<CFGToken> tokens = {
      CFGToken(CFGTokenType::kStart, 0),
      CFGToken(CFGTokenType::kWhile, 1),
      CFGToken(CFGTokenType::kRead, 2),
      CFGToken(CFGTokenType::kIf, 3),
      CFGToken(CFGTokenType::kAssign, 4),
      CFGToken(CFGTokenType::kThenEnd, 0),
      CFGToken(CFGTokenType::kAssign, 5),
      CFGToken(CFGTokenType::kWhile, 6),
      CFGToken(CFGTokenType::kRead, 7),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kElseEnd, 0),
      CFGToken(CFGTokenType::kPrint, 8),
      CFGToken(CFGTokenType::kWhileEnd, 0),
      CFGToken(CFGTokenType::kAssign, 9),
      CFGToken(CFGTokenType::kPrint, 10),
      CFGToken(CFGTokenType::kEnd, 0)
    };

    cfg::CFG cfg = cfg::CFG::GenerateCfg(tokens);
    //start
    shared_ptr<cfg::GraphNode> start = make_shared<cfg::GraphNode>(cfg::NodeType::START, 0);

    //line 1
    shared_ptr<cfg::GraphNode> while1_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILE, 1);
    while1_stmt->stmt_type_.insert(make_pair(1, EntityIdentifier::kWhile));

    //line 2
    shared_ptr<cfg::GraphNode> read2_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 2);
    read2_stmt->stmt_type_.insert(make_pair(2, EntityIdentifier::kRead));

    //line 3
    shared_ptr<cfg::GraphNode> if_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IF, 3);
    if_stmt->stmt_type_.insert(make_pair(3, EntityIdentifier::kIf));

    //line 4
    shared_ptr<cfg::GraphNode> assign4_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 4);
    assign4_stmt->stmt_type_.insert(make_pair(4, EntityIdentifier::kAssign));

    //then end
    shared_ptr<cfg::GraphNode> then_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::THENEND, 0);

    //line 5
    shared_ptr<cfg::GraphNode> assign5_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 5);
    assign5_stmt->stmt_type_.insert(make_pair(5, EntityIdentifier::kAssign));

    //line 6
    shared_ptr<cfg::GraphNode> while6_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILE, 6);
    while6_stmt->stmt_type_.insert(make_pair(6, EntityIdentifier::kWhile));

    //line 7
    shared_ptr<cfg::GraphNode> read7_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 7);
    read7_stmt->stmt_type_.insert(make_pair(7, EntityIdentifier::kRead));

    //while end
    shared_ptr<cfg::GraphNode> while6_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILEEND, 0);

    //if end
    shared_ptr<cfg::GraphNode> if_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::IFEND, 0);

    //line 8
    shared_ptr<cfg::GraphNode> print_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 8);
    print_stmt->stmt_type_.insert(make_pair(8, EntityIdentifier::kPrint));

    //while end
    shared_ptr<cfg::GraphNode> while1_end_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::WHILEEND, 0);

    //line 9 - 10
    shared_ptr<cfg::GraphNode> assign_print_stmt = make_shared<cfg::GraphNode>(cfg::NodeType::STMT, 9);
    assign_print_stmt->end_ = 10;
    assign_print_stmt->stmt_type_.insert(make_pair(9, EntityIdentifier::kAssign));
    assign_print_stmt->stmt_type_.insert(make_pair(10, EntityIdentifier::kPrint));

    //end 
    shared_ptr<cfg::GraphNode> end_node = make_shared<cfg::GraphNode>(cfg::NodeType::END, 0);

    //connect them
    start->next_node_ = while1_stmt;
    while1_stmt->next_node_ = read2_stmt;
    read2_stmt->next_node_ = if_stmt;
    if_stmt->next_node_ = assign4_stmt;
    if_stmt->alternative_node_ = assign5_stmt;

    assign4_stmt->next_node_ = then_end_stmt;
    assign5_stmt->next_node_ = while6_stmt;
    while6_stmt->next_node_ = read7_stmt;
    while6_stmt->alternative_node_ = if_end_stmt;
    read7_stmt->next_node_ = while6_end_stmt;
    while6_end_stmt->next_node_ = while6_stmt;

    then_end_stmt->next_node_ = if_end_stmt;
    if_end_stmt->next_node_ = while1_end_stmt;
    while1_end_stmt->next_node_ = while1_stmt;

    while1_stmt->alternative_node_ = assign_print_stmt;
    assign_print_stmt->next_node_ = end_node;

    cfg::CFG cfg_ans(start);

    REQUIRE(cfg.equal(cfg_ans));
  }
}

