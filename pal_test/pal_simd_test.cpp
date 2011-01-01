#include "libpal/libpal.h"

static float RelativeError(palSimd true_solution, palSimd approx_solution) {
  palSimd diff = palSimdSub(true_solution, approx_solution);
  palSimd abs_diff = palSimdAbsolute(diff);
  palSimd true_solution_length = palSimdLength(true_solution);
  palSimd absolute_error = palSimdLength(abs_diff);
  palSimd relative_error = palSimdDiv(absolute_error, true_solution_length);
  return palSimdGetX(relative_error);
}

static float AbsoluteError(palSimd true_solution, palSimd approx_solution) {
  palSimd diff = palSimdSub(true_solution, approx_solution);
  palSimd abs_diff = palSimdAbsolute(diff);
  palSimd absolute_error = palSimdLength(abs_diff);
  return palSimdGetX(absolute_error);
}

bool PalSimdDotCheck(palSimd a, palSimd b, const float* result) {
  palSimd dot = palSimdDot(a, b);
  float* dot_array = palSimdGetArray(&dot);
  for (int i = 0; i < 4; i++) {
    if (dot_array[i] != result[i]) {
      palBreakHere();
      return false;
    }
  }
  return true;
}

bool PalSimdCrossCheck(palSimd a, palSimd b, const float* result) {
  palSimd cross = palSimdCross(a, b);
  float* cross_array = palSimdGetArray(&cross);
  for (int i = 0; i < 4; i++) {
    if (cross_array[i] != result[i]) {
      palBreakHere();
      return false;
    }
  }
  return true;
}

bool PalSimdCrossTest() {  
  float result_x[4] = { 1.0f, 0.0f, 0.0f, 0.0f};
  float result_y[4] = { 0.0f, 1.0f, 0.0f, 0.0f};
  float result_z[4] = { 0.0f, 0.0f, 1.0f, 0.0f};
  float result_0[4] = { 0.0f, 0.0f, 0.0f, 0.0f};
  float result_negx[4] = { -1.0f, 0.0f, 0.0f, 0.0f};
  float result_negy[4] = { 0.0f, -1.0f, 0.0f, 0.0f};
  float result_negz[4] = { 0.0f, 0.0f, -1.0f, 0.0f};

  PalSimdCrossCheck(palSimd_UNIT_X, palSimd_UNIT_Y, result_z);
  PalSimdCrossCheck(palSimd_UNIT_Y, palSimd_UNIT_Z, result_x);
  PalSimdCrossCheck(palSimd_UNIT_Z, palSimd_UNIT_X, result_y);

  PalSimdCrossCheck(palSimd_UNIT_Y, palSimd_UNIT_X, result_negz);
  PalSimdCrossCheck(palSimd_UNIT_Z, palSimd_UNIT_Y, result_negx);
  PalSimdCrossCheck(palSimd_UNIT_X, palSimd_UNIT_Z, result_negy);

  PalSimdCrossCheck(palSimd_UNIT_X, palSimd_UNIT_X, result_0);
  PalSimdCrossCheck(palSimd_UNIT_Y, palSimd_UNIT_Y, result_0);
  PalSimdCrossCheck(palSimd_UNIT_Z, palSimd_UNIT_Z, result_0);

  {
    palSimd a = { 3.0f, -3.0f, 1.0f, 0.0f };
    palSimd b = { 4.0f, 9.0f, 2.0f, 0.0f };
    float result[4] = { -15.0f, -2.0f, 39.0f, 0.0f};
    PalSimdCrossCheck(a, b, result);
  }

  return true;
}

bool palSimdMaskTest() {
  palSimd num = palSimdSetXYZW(1.0, 2.0, 3.0, 4.0);
  palSimd numX = palSimdSetXYZW(1.0, 0.0, 0.0, 0.0);
  palSimd numY = palSimdSetXYZW(0.0, 2.0, 0.0, 0.0);
  palSimd numZ = palSimdSetXYZW(0.0, 0.0, 3.0, 0.0);
  palSimd numW = palSimdSetXYZW(0.0, 0.0, 0.0, 4.0);

  palSimd Xmask = palSimdMakeMask(0xffffffff, 0x0, 0x0, 0x0);
  palSimd Ymask = palSimdMakeMask(0x0, 0xffffffff, 0x0, 0x0);
  palSimd Zmask = palSimdMakeMask(0x0, 0x0, 0xffffffff, 0x0);
  palSimd Wmask = palSimdMakeMask(0x0, 0x0, 0x0, 0xffffffff);

  palSimd maskedX = palSimdSelect(Xmask, num, palSimdZero());
  palSimd maskedY = palSimdSelect(Ymask, num, palSimdZero());
  palSimd maskedZ = palSimdSelect(Zmask, num, palSimdZero());
  palSimd maskedW = palSimdSelect(Wmask, num, palSimdZero());
  return true;
}

bool palSimdSwizzleTest() {
  palSimd num = palSimdSetXYZW(1.0, 2.0, 3.0, 4.0);
  palSimd allX = palSimdSwizzle(num, 0, 0, 0, 0);
  palSimd allY = palSimdSwizzle(num, 1, 1, 1, 1);
  palSimd allZ = palSimdSwizzle(num, 2, 2, 2, 2);
  palSimd allW = palSimdSwizzle(num, 3, 3, 3, 3);
  palSimd reversed = palSimdSwizzle(num, 3, 2, 1, 0);
  palSimd same = palSimdSwizzle(num, 0, 1, 2, 3);
  return true;
}

bool PalSimdDotTest() {
  palSimd nums = palSimdSetXYZW(1.0, 2.0, 3.0, 4.0);
  float result_nums[4] = { 30.0f, 30.0f, 30.f, 30.0f};
  
  if (PalSimdDotCheck(nums, nums, result_nums) == false) {
    palBreakHere();
  }

  float result_unit[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
  if (PalSimdDotCheck(palSimd_UNIT_X, palSimd_UNIT_X, result_unit) == false) {
    palBreakHere();
  }

  if (PalSimdDotCheck(palSimd_UNIT_Y, palSimd_UNIT_Y, result_unit) == false) {
    palBreakHere();
  }

  if (PalSimdDotCheck(palSimd_UNIT_Z, palSimd_UNIT_Z, result_unit) == false) {
    palBreakHere();
  }

  if (PalSimdDotCheck(palSimd_UNIT_W, palSimd_UNIT_W, result_unit) == false) {
    palBreakHere();
  }

  return true;
}

bool PalSimdTest()
{
  palSimdSwizzleTest();
  palSimdMaskTest();
  PalSimdDotTest();
  PalSimdCrossTest();

  palSimd zeros = palSimdZero();
  palSimd ones = palSimdSetXYZ0(1.0, 1.0, 1.0);
  palSimd nums = palSimdSetXYZW(1.0, 2.0, 3.0, 4.0);
  palSimd splatted = palSimdSplat(3.14159f);

  {
    float x = 4.0;
    float y = 8.0;
    float z = 12.0;
    float w = 16.0;

    palSimd num = palSimdZero();

    num = palSimdSetX(num, x);
    num = palSimdSetY(num, y);
    num = palSimdSetZ(num, z);
    num = palSimdSetW(num, w);

    palAssertBreak(palSimdGetX(num) == x);
    palAssertBreak(palSimdGetY(num) == y);
    palAssertBreak(palSimdGetZ(num) == z);
    palAssertBreak(palSimdGetW(num) == w);

    float num_in[4] = { x,y,z,w};
    float num_out[4];

    palSimdStoreUnaligned(num, &num_out[0]);
    for (int i = 0; i < 4; i++)
    {
      palAssertBreak(num_in[i] == num_out[i]);
    }
  }

  {
    float f_in[4] = { 1, 2, 3, 4};
    float f_out[4] = { 0, 0, 0, 0};
    palSimd a = palSimdLoadUnaligned(&f_in[0]);
    palSimdStoreUnaligned(a, &f_out[0]);
    for (int i = 0; i < 4; i++)
    {
      palAssertBreak(f_in[i] == f_out[i]);
    }
  }

  return true;
}