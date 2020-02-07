
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
auto list = [](auto... xs) {
    return [=](auto callable_) {
        return callable_(xs...);
    };
};

/*
return result of a call to first argument
or if no args are give return tuple of 
arguments originaly given
*/
auto listx = [](auto... xs) {
    return [=](auto... args_) {
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
return list with one element
*/
auto head = [](auto list_) {
    return list_(
        [=](auto first_, auto... args) {
            return list(first_);
        });
};
/*
take all but the first element from list argument
return list with them elements
*/
auto tail = [](auto list_) {
    return list_(
        [=](auto first_, auto... args) {
            return list(args...);
        });
};
/* make tuple from a list */
auto listuple = [&](auto list_) {
    //
    return list_(
        [=](auto... args) {
            return std::make_tuple(args...);
        });
};

/* 
get first element from a list
no tuple necessary
 */
decltype(auto) list_first = [&](auto list_) {
    //
    return list_(
        [=](auto first_, auto... args) {
            return (first_);
        });
};

///--------------------------------------------------
static int worker(int, char **)
{
    auto L1 = list("HEAD", true, 3.14, "TAIL");
    auto H = head(L1);
    auto T = tail(L1);
    auto tupson = listuple(L1);
    auto first = std::get<0>(tupson);

    auto firstico = list_first(L1);

    auto LX = listx("HEAD", true, 3.14, "TAIL");
    auto firston = LX();
    auto first_2 = std::get<0>(firston);

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
