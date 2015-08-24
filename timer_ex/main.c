#include "stm32f4xx.h"
#include "lib.h"

#define PLL_M 8
#define PLL_N 336
#define PLL_P 2
#define PLL_Q 7

void EnablePll()
{
    /* Set up SYSCLK to 168 MHz */
    

    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY))
        continue;
    
    /* Power supply setup */
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;

    /*
     * AHB  = SYSCLK / 1 = 168 MHz
     * APB2 = SYSCLK / 2 = 84 MHz
     * APB1 = SYSCLK / 4 = 42 MHz
     */
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1  |
                 RCC_CFGR_PPRE2_DIV2 |
                 RCC_CFGR_PPRE1_DIV4;
    
    /* * PLLCLK = HSI(16 MHz) / M * N / P */
    RCC->PLLCFGR = PLL_M | (PLL_N << 6) | (((PLL_P >> 1) -1) << 16) |
                   RCC_PLLCFGR_PLLSRC_HSE | (PLL_Q << 24);

    /* PLL ON */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait until PLL is locked up */
    while (!(RCC->CR & RCC_CR_PLLRDY))
        continue;

    /* Enable I-Cache and D-Cache and flash latency */
    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;

    /* Switch SYSCLK source to PLLCLK */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;

    /* Ensure the register updated */
    while ((RCC->CFGR & RCC_CFGR_SW) != RCC_CFGR_SW_PLL)
        continue;
}

void MemoryInit()
{
    extern unsigned int rodata_end;
    extern unsigned int data_start;
    extern unsigned int data_end;
    extern unsigned int sram_start;
    extern unsigned int sram_end;
    extern void (*vector_table[])();
    extern size_t vector_table_count;

    /* Clear SRAM area with zero (bss section is cleared here) */
    memset((void *)&sram_start, 0, &sram_end - &sram_start);
    
    /* Copy the exception vector into SRAM area */
    memcpy((void *)&sram_start, (void *)vector_table, vector_table_count);

    /* Copy .data section into SRAM area */
    memcpy((void *)&sram_start + vector_table_count, (void *)&rodata_end,
           (void *)&data_end - (void *)&data_start);
}

void Reset_Handler()
{
    SystemInit();
    
    EnablePll();

    SystemCoreClockUpdate();

    MemoryInit();

    /* Enable clock for each peripherals */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* PC12~PC15 pin is assigned for GPIO output */
    GPIOD->MODER = 0x55550000;
    GPIOD->ODR   = 0x0000F000;
    
    /* Timer interrupting at regular interval */
    TIM2->PSC  = 1000-1;       /* Timer clock  84 MHz / PSC = 84 KHz */
    TIM2->ARR  = 84*1000-1;    /* Auto reload: 84 KHz / ARR =  1 Hz */
    TIM2->DIER = TIM_DIER_UIE; /* Enable update interrupt */
    TIM2->EGR  = TIM_EGR_UG;   /* Initialize counter */
    TIM2->SR   = 0;            /* Clear all interrupts */
    TIM2->CR1 |= TIM_CR1_CEN;  /* Enable counter */

    NVIC_SetPriority(TIM2_IRQn, 2);
    NVIC_EnableIRQ(TIM2_IRQn);

    while (1)
        continue;
}

void TIM2_IRQHandler()
{
    if (TIM2->SR & TIM_SR_UIF) {
        TIM2->SR = ~TIM_SR_UIF;   /* Clear update interrupt */
        GPIOD->ODR ^= 0x0000F000; /* Toggle GPIO output */
    }
}
