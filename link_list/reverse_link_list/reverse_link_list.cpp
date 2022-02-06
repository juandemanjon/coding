// Given the head of a singly linked list, reverse the list, and return the reversed list.
//
// Input: head = [1,2,3,4,5]
// Output: [5,4,3,2,1]
//
// Input: head = [1,2]
// Output: [2,1]
//
// Input: head = []
// Output: []

//      1 -> 2 -> 3 -> 4 -> 5
// ^    ^    ^  
// P    C    F
//
// P <- C, P = C, C = F   
//
//   <- 1    2 -> 3 -> 4 -> 5
//      ^    ^    ^  
//      P    C    F
//
//      P <- C, P = C, C = F  
//
//   <- 1 <- 2    3 -> 4 -> 5
//           ^    ^    ^  
//           P    C    F
//


#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <iostream>


struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

ListNode* reverseList(ListNode* head) {
    ListNode *P = nullptr;
    ListNode *C = head;
    ListNode *F = nullptr;
       
    if (!C || !C->next)
        return head;
    
    while (C)
    {
        F = C->next;
        
        C->next = P;
        P = C;
        C = F;
    }
    
    return P;
}


TEST_CASE("Reverse Link List")
{   
    REQUIRE(reverseList(nullptr) == nullptr);
    
    ListNode n1(1);
    ListNode n2(2);
    ListNode n3(3);
    ListNode n4(4);
    ListNode n5(5);

    {
        auto result = reverseList(nullptr);
        REQUIRE(result == nullptr);
    }    
    {
        n1.next = nullptr;
        auto result = reverseList(&n1);
        REQUIRE(result == &n1);
    }
    {
        n1.next = &n2;
        n2.next = nullptr;
        auto result = reverseList(&n1);
        REQUIRE(result);
        CHECK(result->val == 2);
        REQUIRE(result->next);
        CHECK(result->next->val == 1);
        CHECK(result->next->next == nullptr);
    }
    {
        n1.next = &n2;
        n2.next = &n3;
        n3.next = nullptr;
        auto result = reverseList(&n1);
        REQUIRE(result);
        CHECK(result->val == 3);
        REQUIRE(result->next);
        CHECK(result->next->val == 2);
        REQUIRE(result->next->next);
        CHECK(result->next->next->val == 1);
        CHECK(result->next->next->next == nullptr);
    }
    {
        n1.next = &n2;
        n2.next = &n3;
        n3.next = &n4;
        n4.next = &n5;
        n5.next = nullptr;
        auto result = reverseList(&n1);
        REQUIRE(result);
        CHECK(result->val == 5);
        REQUIRE(result->next);
        CHECK(result->next->val == 4);
        REQUIRE(result->next->next);
        CHECK(result->next->next->val == 3);
        REQUIRE(result->next->next->next);
        CHECK(result->next->next->next->val == 2);
        REQUIRE(result->next->next->next->next);
        CHECK(result->next->next->next->next->val == 1);
        CHECK(result->next->next->next->next->next == nullptr);
    }
    
    
}