#include <string>
#include <vector>
class Palindrome{
public:
    std::string LongestPalindrome(std::string str){
        if (str == "") {
            return "";
        }
        auto n = str.size();
        int _max_lens = 1;
        int _begin = 0;

        std::vector<std::vector<int>> dp(n, std::vector<int>(n));
        for (int i = 0; i < n; i++) {
            dp[i][i] = true;
        }

        for(int left=2; left<=n; left++){
            for(int i=0; i<n; i++){
                int j = left + i - 1;
                if(j >= n){
                    break;
                }
                if(str[i] != str[j]){
                    dp[i][j] = false;
                }else{
                     if (j - i < 3) {
                        dp[i][j] = true;
                    } else {
                        dp[i][j] = dp[i + 1][j - 1];
                    }
                }
                if (dp[i][j] && j - i + 1 > _max_lens) {
                    _max_lens = j - i + 1;
                    _begin = i;
                }
            }
        }

        return str.substr(_begin, _max_lens);
    }
    bool IsPalindrome(const std::string str){
        if (str == ""){
            return true;
        }
        auto n = str.size();
        auto _begin = str.begin();
        auto _end = --str.end();
        for (int i=0; i<n; ++i){
            if (*_begin != *_end){
                return false;
            }
            if(_end - _begin <= 0){
                return true;
            }
            _begin ++;
            _end --;
        }
        return true;
    }

};


