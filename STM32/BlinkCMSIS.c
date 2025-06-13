#include "stm32f1xx.h"  // CMSIS header for STM32F1 series

void delay(volatile uint32_t time)
{
    while (time--) __NOP();
}

void SystemClock_Config(void)
{
    // 1. Enable HSE and wait for it to be ready
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    // 2. Configure Flash prefetch and latency
    FLASH->ACR |= FLASH_ACR_PRFTBE; // Prefetch enable
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2; // 2 wait states

    // 3. Set PLL source and multiplier: HSE * 9 = 72 MHz
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;

    // 4. Configure bus prescalers
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;    // AHB
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;   // APB1 (max 36 MHz)
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;   // APB2

    // 5. Enable PLL and wait
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 6. Switch system clock to PLL
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void GPIO_Config(void)
{
    // Enable clock to GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    // Configure PA5 as output push-pull, 2 MHz
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5); // Clear settings
    GPIOA->CRL |= (0x2 << GPIO_CRL_MODE5);           // Output mode 2 MHz
    GPIOA->CRL |= (0x0 << GPIO_CRL_CNF5);            // Push-pull
}

int main(void)
{
    SystemClock_Config();  // Set SYSCLK = 72 MHz
    GPIO_Config();         // Init LED pin

    while (1)
    {
        GPIOA->ODR ^= (1 << 5); // Toggle PA5
        delay(500000);
    }
}