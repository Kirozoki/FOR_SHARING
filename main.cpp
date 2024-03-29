#include <iostream>
#include <cassert>
#include <vector>

using namespace std;

template <typename X>
void RunTestImpl(X& func, const string& func_name ) {
    func();
    cerr << func_name << " OK"s << endl;
}

#define RUN_TEST(func) RunTestImpl(func, #func)

struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode* next1) : val(x), next(next1) {}
};

class Solution {
public:
    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
        
        int remain_num = 0;
        ListNode* n_ptr = nullptr; 
        ListNode* initial_ptr = nullptr;
        while( l1 != nullptr || l2 != nullptr) {
            int sum_nums = 0;
            if (l1 && l2) {
                sum_nums = (*l1).val + (*l2).val + remain_num;
            } else if (!l1) {
                sum_nums = (*l2).val + remain_num;
            } else {
                sum_nums = (*l1).val + remain_num;
            } 

            remain_num = sum_nums < 10 ? 0 : sum_nums / 10;
            sum_nums %= 10;
            ListNode* new_ptr = new ListNode({sum_nums, nullptr});
            if (n_ptr) {
            n_ptr->next = new_ptr;
            } else {
                initial_ptr = new_ptr;
            }
            n_ptr = new_ptr;
            if (l1) {
            l1 = ((*l1).next);}
            if (l2) {
            l2 = ((*l2).next);}
        }
        if (remain_num > 0) {
            ListNode* new_ptr = new ListNode({remain_num, nullptr});
            n_ptr->next = new_ptr;
        }
        return initial_ptr;
    }
};

void Test() {
    Solution S;
    {
    int a1 =2;
    int a2 =4;
    int a3 =3;
    ListNode a_3 = {a3};
    ListNode a_2 = {a2, &a_3};
    ListNode a_1 = {a1, &a_2};

    int b1 =5;
    int b2 =6;
    int b3 =4;
    ListNode b_3 = {b3};
    ListNode b_2 = {b2, &b_3};
    ListNode b_1 = {b1, &b_2};
    
    ListNode s = *(S.addTwoNumbers(&a_1, &b_1));
    assert(s.val == 7);
    }
    {
    int a1 =9, a2 =9, a3 =9, a4 =9, a5 =9, a6 =9, a7 =9;

    ListNode a_7 = {a7};
    ListNode a_6 = {a6, &a_7};
    ListNode a_5 = {a5, &a_6};
    ListNode a_4 = {a4, &a_5};
    ListNode a_3 = {a3, &a_4};
    ListNode a_2 = {a2, &a_3};
    ListNode a_1 = {a1, &a_2};

    int b1 =9;
    int b2 =9;
    int b3 =9;
    int b4 =9;
    ListNode b_4 = {b4};
    ListNode b_3 = {b3, &b_4};
    ListNode b_2 = {b2, &b_3};
    ListNode b_1 = {b1, &b_2};
    
    ListNode* s = S.addTwoNumbers(&a_1, &b_1);
    vector<int> v_nums = {8,9,9,9,0,0,0,1};
    int i = 0;
    while (s != nullptr) {
        int value = (*s).val;
        cout << value << " ";
        assert(value == v_nums[i]);
        ++i;
        s = (*s).next;
    }
    }
}

int main() {
    RUN_TEST(Test);
}