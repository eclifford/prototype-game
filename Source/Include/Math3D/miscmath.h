#pragma once

bool IsPowerOfTwo(unsigned int i)
{
   return ((i == 1) || !(i & (i -1)));   
}

unsigned int ClosestLargerPowerOfTwo(unsigned int i)
{
   if (IsPowerOfTwo(i))
      return i;

   if (i == 0)
      return 0;

   unsigned int Mask = 0x01 << 31;

   while (!(Mask & i))
   {
      Mask >>= 1;
   }

   return Mask << 1;
}