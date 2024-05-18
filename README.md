# FOR_SHARING
Ошибка компиляции:
In file included from /exposed/submission/main.cpp:1:
/exposed/submission/simple_vector.h: In function ‘bool operator!=(SimpleVector<Type>, SimpleVector<Type>)’:
/exposed/submission/simple_vector.h:262:19: error: logical not is only applied to the left hand side of comparison [-Werror=logical-not-parentheses]
  262 |     return (!left == right);
      |                   ^~
/exposed/submission/simple_vector.h:262:13: note: add parentheses around left hand side expression to silence this warning
  262 |     return (!left == right);
      |             ^~~~~
      |             (    )
/exposed/submission/simple_vector.h: In function ‘bool operator>=(SimpleVector<Type>, SimpleVector<Type>)’:
/exposed/submission/simple_vector.h:277:19: error: logical not is only applied to the left hand side of comparison [-Werror=logical-not-parentheses]
  277 |     return (!left < right);
      |                   ^
/exposed/submission/simple_vector.h:277:13: note: add parentheses around left hand side expression to silence this warning
  277 |     return (!left < right);
      |             ^~~~~
      |             (    )
/exposed/submission/simple_vector.h: In function ‘bool operator<=(SimpleVector<Type>, SimpleVector<Type>)’:
/exposed/submission/simple_vector.h:282:20: error: logical not is only applied to the left hand side of comparison [-Werror=logical-not-parentheses]
  282 |     return (!right < left);
      |                    ^
/exposed/submission/simple_vector.h:282:13: note: add parentheses around left hand side expression to silence this warning
  282 |     return (!right < left);
      |             ^~~~~~
      |             (     )
In file included from /usr/include/c++/12/string:50,
                 from /usr/include/c++/12/bits/locale_classes.h:40,
                 from /usr/include/c++/12/bits/ios_base.h:41,
                 from /usr/include/c++/12/ios:42,
                 from /usr/include/c++/12/ostream:38,
                 from /usr/include/c++/12/iostream:39,
                 from /exposed/submission/array_ptr.h:3,
                 from /exposed/submission/simple_vector.h:3:
/usr/include/c++/12/bits/stl_algobase.h: In instantiation of ‘static constexpr _BI2 std::__copy_move_backward<false, false, std::random_access_iterator_tag>::__copy_move_b(_BI1, _BI1, _BI2) [with _BI1 = X*; _BI2 = X*]’:
/usr/include/c++/12/bits/stl_algobase.h:756:17:   required from ‘constexpr _BI2 std::__copy_move_backward_a2(_BI1, _BI1, _BI2) [with bool _IsMove = false; _BI1 = X*; _BI2 = X*]’
/usr/include/c++/12/bits/stl_algobase.h:769:51:   required from ‘constexpr _BI2 std::__copy_move_backward_a1(_BI1, _BI1, _BI2) [with bool _IsMove = false; _BI1 = X*; _BI2 = X*]’
/usr/include/c++/12/bits/stl_algobase.h:800:5:   required from ‘constexpr _OI std::__copy_move_backward_a(_II, _II, _OI) [with bool _IsMove = false; _II = X*; _OI = X*]’
/usr/include/c++/12/bits/stl_algobase.h:858:7:   required from ‘constexpr _BI2 std::copy_backward(_BI1, _BI1, _BI2) [with _BI1 = X*; _BI2 = X*]’
/exposed/submission/simple_vector.h:202:31:   required from ‘Type* SimpleVector<Type>::Insert(Iterator, Type&&) [with Type = X; Iterator = X*]’
/exposed/submission/main.cpp:125:13:   required from here
/usr/include/c++/12/bits/stl_algobase.h:702:25: error: use of deleted function ‘X& X::operator=(const X&)’
  702 |             *--__result = *--__last;
      |             ~~~~~~~~~~~~^~~~~~~~~~~
/exposed/submission/main.cpp:18:8: note: declared here
   18 |     X& operator=(const X& other) = delete;
      |        ^~~~~~~~
/usr/include/c++/12/bits/stl_algobase.h: In instantiation of ‘static constexpr _OI std::__copy_move<false, false, std::random_access_iterator_tag>::__copy_m(_II, _II, _OI) [with _II = X*; _OI = X*]’:
/usr/include/c++/12/bits/stl_algobase.h:492:12:   required from ‘constexpr _OI std::__copy_move_a2(_II, _II, _OI) [with bool _IsMove = false; _II = X*; _OI = X*]’
/usr/include/c++/12/bits/stl_algobase.h:522:42:   required from ‘constexpr _OI std::__copy_move_a1(_II, _II, _OI) [with bool _IsMove = false; _II = X*; _OI = X*]’
/usr/include/c++/12/bits/stl_algobase.h:530:31:   required from ‘constexpr _OI std::__copy_move_a(_II, _II, _OI) [with bool _IsMove = false; _II = X*; _OI = X*]’
/usr/include/c++/12/bits/stl_algobase.h:620:7:   required from ‘constexpr _OI std::copy(_II, _II, _OI) [with _II = X*; _OI = X*]’
/exposed/submission/simple_vector.h:209:32:   required from ‘Type* SimpleVector<Type>::Insert(Iterator, Type&&) [with Type = X; Iterator = X*]’
/exposed/submission/main.cpp:125:13:   required from here
/usr/include/c++/12/bits/stl_algobase.h:385:25: error: use of deleted function ‘X& X::operator=(const X&)’
  385 |               *__result = *__first;
      |               ~~~~~~~~~~^~~~~~~~~~
/exposed/submission/main.cpp:18:8: note: declared here
   18 |     X& operator=(const X& other) = delete;
      |        ^~~~~~~~
cc1plus: all warnings being treated as errors

