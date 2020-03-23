
/// MSVC, C++17
/// one consumer, 1..N futures
/// it is important to realize things are NOT happening in-order
/// in multi-threading apps.
/// For example single consumer bellow might start before
/// any of the futures
/// the key in makin things the way you want, is synchronization
#include <tuple>
#include <typeinfo>
#include <stdio.h>
#include <stdlib.h>

///--------------------------------------------------
/*
The key abstraction : list 
*/
auto list = [](auto... xs) constexpr
{
    return [=](auto callable_) constexpr
    {
        return callable_(xs...);
    };
};

// this is perhaps questionable
using allowed_list_processor_type = decltype(list);
// and this too?
constexpr auto empty_list = list();

/*

this is not elegant

return result of a call to first argument
or if no args are give return tuple of 
arguments originaly given
*/
auto listx = [](auto... xs) constexpr
{
    return [=](auto... args_) constexpr
    {
        if constexpr (sizeof...(args_))
        {
            auto callables = {args_...};
            return callables.begin()(xs...);
        }
        else
        {
            return std::make_tuple(xs...);
        }
    };
};
/*
take the first element from list argument
return list with that one element
*/
auto head = [](auto list_) constexpr
{
    return list_(
        [=](auto first_, auto... args) constexpr {
            return list(first_);
        });
};
/*
take all but the first element from list argument
return list with them elements
*/
auto tail = [](auto list_) constexpr
{
    return list_(
        [=](auto first_, auto... args) constexpr {
            return list(args...);
        });
};
/*
return size of the list
*/
auto size = [](auto list_) constexpr
{
    return list_(
        [=](auto... args) constexpr {
            return sizeof...(args);
        });
};
/* make tuple from a list */
auto listuple = [&](auto list_) constexpr
{
    //
    return list_(
        [=](auto... args) constexpr {
            return std::make_tuple(args...);
        });
};

/* 
get first element from a list
no tuple necessary
 */
decltype(auto) first_element = [&](auto list_) constexpr
{
    //
    return list_(
        [=](auto first_, auto... args) constexpr {
            return (first_);
        });
};

auto nth_head = [&](unsigned idx_, auto list_) constexpr
{
    //
    auto get_the_nth_head = [&](unsigned walker_, auto first, auto ... tail_) constexpr
    {
        if (walker_ == idx_)
        {
            return tail_;
        }
        else
        {
            return get_the_nth_head(walker_ + 1, tail_ ...);
        }
    };
    //
    return list_(
        [=]( auto... args_) constexpr {
            return get_the_nth_head(0, args_ ...);
        });
};

/* 
get nth element from a list
no tuple necessary
on error empty list is retuned
 */
// decltype(auto) nth_element = [&](unsigned idx_, auto list_)
// {
//     // helper
//     auto get_the_nth_head = [&](unsigned counter_, auto tail_)
//     {
//         if (counter_ == idx_)
//         {
//             return head(tail_);
//         }
//         else
//         {
//             return (counter_ + 1, tail(tail_));
//         }
//     };
//     //
//     return list_(
//         [=](auto first_, auto... args)  {
//             // empty list
//             if constexpr (size(list_) < 1)
//             {
//                 return list_;
//             }
//             // single element list
//             else if constexpr (size(list_) < 2)
//             {
//                 // basically ignore the idx_
//                 return first(list_);
//             }
//             // two or more elements
//             else
//             {
//                 if (idx_ > size(list_))
//                     return list(); // empty list
//                 // get the heads until the nth element
//                 auto nth_head = get_the_nth_head(0, list_);
//                 return first_element(nth_head);
//             }
//         });
// };

///--------------------------------------------------
static int worker(int, char **)
{
    constexpr auto L1 = list("HEAD", true, 3.14, "TAIL");
    constexpr auto H = head(L1);
    constexpr auto S = size(H);
    constexpr auto T = tail(L1);
    constexpr auto tupson = listuple(L1);
    constexpr auto first = std::get<0>(tupson);

    constexpr auto firstico = first_element(L1);

    // empty list?
    constexpr auto empty_list = list();
    constexpr auto S0 = size(empty_list);
    // does not compile
    // constexpr auto empty_head = head(empty_list);

    constexpr auto lonely_list = list(42);
    constexpr auto H42 = head(lonely_list);

    auto nth_ = nth_head(2, L1);

    // constexpr auto LX = listx("HEAD", true, 3.14, "TAIL");
    // constexpr auto firston = LX();
    // constexpr auto first_2 = std::get<0>(firston);

    // auto tup = tupson();

    return EXIT_SUCCESS;
} // worker

///--------------------------------------------------
/// we can build with or without exceptions enabled
///
int main(int argc, char **argv)
{
#if (_HAS_EXCEPTIONS > 0)
    try
    {
#endif // _HAS_EXCEPTIONS
        return worker(argc, argv);
#if (_HAS_EXCEPTIONS > 0)
    }
    catch (std::future_error const &err)
    {
        printf("\nstd future_error: %s", err.what());
    }
    catch (std::system_error &syserr_)
    {
        printf("\nstd system_error: %s", syserr_.what());
    }
    catch (std::exception &ex_)
    {
        printf("\nstd exception: %s", ex_.what());
    }
    catch (...)
    {
        printf("\nUnknown exception");
    }
#endif // _HAS_EXCEPTIONS
    return EXIT_SUCCESS;
}
