// In real world horses neigh, and you can count them by listening to them.
// 
// For this problem you will be given an input string consisting of lowercases letters which represents combination of neigh of // different horses.
// 
// You need to return an integer corresponding to minimum number of distinct horses which can produce the given sequence.
// 
// If the input string is not a combination of valid neigh from different horses return -1.
// 
// Example :
// 
// Input : "nei"
// Output : -1
// Explanation: Not a valid neigh.
// Input : "neighneigh"
// Output : 1
// Explanation: Single horse yelling neigh two times.
// Input : "neingeighh"
// Output : 2
// Explanation: Second horse can be seen speaking before the first one finished.

//
//    n   2  1   
//    e   1  0
//    i   1  0
//    g   1  0
//    h   <
//

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

int solve(std::string_view input)
{
    if (input.length() % 5 != 0) 
        return -1;
    
    
    int counter[5] = {0};
    
    int max = -1;
    
    for (auto c : input)
    {
        switch (c)
        {
            case 'n':
                counter[0]++;
                max = std::max(max, counter[0]);
                break;
            case 'e':
                counter[1]++;
                if (counter[1] > counter[0] )
                    return -1;
                break;
            case 'i':
                counter[2]++;
                if (counter[2] > counter[1] )
                    return -1;
                break;
            case 'g':
                counter[3]++;
                if (counter[3] > counter[2] )
                    return -1;
                break;
            case 'h':
                counter[4]++;
                if (counter[4] > counter[3] )
                    return -1;                
                counter[0]--;
                counter[1]--;
                counter[2]--;
                counter[3]--;
                counter[4]--;
                break;
            default:
                return -1;
        }
        
    }
    if (counter[0] != counter[1] || counter[1] != counter[2] || counter[2] != counter[3])
        return -1;
    
    return max;
}

TEST_CASE("Horses Neigh")
{
    struct TestCase
    {
        std::string_view input;
        int expected;
    };
    
    std::vector<TestCase> test_cases = {
        {"",-1}, {"n",-1}, {"neighn",-1}, {"neighnnnnn",-1},
        {"neigh", 1},
        {"neignheigh", 2},
        {"niegnheigh", -1},
        {"neihgneigh", -1},
        {"ngheineigh", -1},
        {"neigneighh", 2},
        };
    
        
    for (const auto& tc : test_cases)
    {
        INFO("input: " << tc.input);
        CHECK(solve(tc.input) == tc.expected);
    }
}