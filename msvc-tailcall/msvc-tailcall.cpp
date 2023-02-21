#include <Windows.h>
#include <stdio.h>
#include <stdint.h>

#include <MinHook.h>

#include "args.h"

// compiled stub
//
// const unsigned char stub[/* 132 */] = {
//     0x48, 0x89, 0x4C, 0x24, 0x08,                             // mov [rsp + 0x8], rcx
//     0x48, 0x89, 0x54, 0x24, 0x10,                             // mov [rsp + 0x10], rdx
//     0x4C, 0x89, 0x44, 0x24, 0x18,                             // mov [rsp + 0x18], r8
//     0x4C, 0x89, 0x4C, 0x24, 0x20,                             // mov [rsp + 0x20], r9
//     0x48, 0x83, 0xEC, 0x48,                                   // sub rsp, 0x48
//     0xF2, 0x0F, 0x11, 0x44, 0x24, 0x28,                       // movsd [rsp + 0x28], xmm0
//     0xF2, 0x0F, 0x11, 0x4C, 0x24, 0x30,                       // movsd [rsp + 0x30], xmm1
//     0xF2, 0x0F, 0x11, 0x54, 0x24, 0x38,                       // movsd [rsp + 0x38], xmm2
//     0xF2, 0x0F, 0x11, 0x5C, 0x24, 0x40,                       // movsd [rsp + 0x40], xmm3
//     0x48, 0x8D, 0x4C, 0x24, 0x28,                             // lea rcx, [rsp + 0x28]
//     0xFF, 0x15, 0x02, 0x00, 0x00, 0x00,                       // call qword ptr [rip + 0x2]
//     0xEB, 0x08,                                               // jmp $+0x8
//     /* 61 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // hook address
//     0xF2, 0x0F, 0x10, 0x5C, 0x24, 0x40,                       // movsd xmm3, [rsp + 0x40]
//     0xF2, 0x0F, 0x10, 0x54, 0x24, 0x38,                       // movsd xmm2, [rsp + 0x38]
//     0xF2, 0x0F, 0x10, 0x4C, 0x24, 0x30,                       // movsd xmm1, [rsp + 0x30]
//     0xF2, 0x0F, 0x10, 0x44, 0x24, 0x28,                       // movsd xmm0, [rsp + 0x28]
//     0x48, 0x83, 0xC4, 0x48,                                   // add rsp, 0x48
//     0x4C, 0x8B, 0x4C, 0x24, 0x20,                             // mov r9, [rsp + 0x20]
//     0x4C, 0x8B, 0x44, 0x24, 0x18,                             // mov r8, [rsp + 0x18]
//     0x48, 0x8B, 0x54, 0x24, 0x10,                             // mov rdx, [rsp + 0x10]
//     0x48, 0x8B, 0x4C, 0x24, 0x08,                             // mov rcx, [rsp + 0x8]
//     0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,                       // jmp qword ptr [rip + 0x0]
//     /* 123 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // original address
//     0xCC                                                      // int 3
// };

// random structs for the example
typedef struct _example_struct
{
    int a;
    int b;
    int c;
    int d;
} example_struct;

typedef struct _result_struct
{
    long long a;
    long long b;
} result_struct;

EXTERN_C VOID tailStub();
EXTERN_C LPVOID pOriginal = NULL;

EXTERN_C VOID pHookfunc(Arguments<result_struct, 7> *args)
{
    printf("rcx value: %d\n", *args->get<int, 0>());
    printf("rdx value: %d\n", *args->get<int, 1>());
    printf("r8 value: %d\n", *args->get<int, 2>());
    printf("xmm3 value: %f\n", *args->get<float, 3>());
    printf("floating point stack value: %f\n", *args->get<float, 4>());
    printf("integer stack value: %d\n", *args->get<int, 5>());

    example_struct *g = args->get<example_struct, 6>();
    printf("struct values:\na: %d\nb: %d\nc: %d\nd: %d\n", g->a, g->b, g->c, g->d);

    // no idea why you would need this as values will
    // be overwritten by the original function anyway
    // but it's here if you need it.
    // note that this is stack allocated by the caller,
    // don't try to access it later on.
    result_struct *result = args->get_return_value();

    // uninitialized stack stuff
    printf("return values:\na: %lld\nb: %lld\n", result->a, result->b);

    result->a = 10;
    result->b = 20;

	// modify whatever
    args->get<example_struct, 6>()->a = 1337;
    *args->get<int, 0>() = 9;
}

#pragma optimize("", off)
__declspec(noinline) result_struct myfunction(int a, int b, int c, float d, float e, int f, example_struct g)
{
    if (d > 10 || e > 8)
    {
        return {0, 0};
    }

    return {a + b + c - f, g.a + g.b + g.c - g.d};
}

int main()
{
    example_struct g = {3, 1, 2, 4};

    result_struct result = myfunction(1, 2, 3, 4.5, -1.5, 6, g);

    printf("myfunction: { %lld, %lld }\n", result.a, result.b);

    if (MH_Initialize() != MH_OK)
    {
        printf("failed #1\n");
        return 1;
    }

    if (MH_CreateHook(myfunction, tailStub, &pOriginal) != MH_OK)
    {
        printf("failed #2\n");
        return 1;
    }

    if (MH_EnableHook(myfunction) != MH_OK)
    {
        printf("failed #3\n");
        return 1;
    }

    result_struct result2 = myfunction(1, 2, 3, 4.5, -1.5, 6, g);

    printf("hooked myfunction: { %lld, %lld }\n", result2.a, result2.b);

    return 0;
}
#pragma optimize("", on)