#include "stm32f10x.h"  // CMSIS header for STM32F1 series

void delay(volatile uint32_t time)
{
    while (time--) __NOP();  // crude delay
}

int main(void)
{
    // 1. Enable internal HSI clock and wait until it's ready
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));

    // 2. Set HSE (external 8 MHz) and enable PLL for 72 MHz system clock
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));

    // 3. Set FLASH wait states for 72 MHz operation
    FLASH->ACR |= FLASH_ACR_PRFTBE;      // Enable prefetch buffer
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    FLASH->ACR |= FLASH_ACR_LATENCY_2;   // 2 wait states

    // 4. Configure PLL: PLLCLK = HSE * 9 = 8MHz * 9 = 72 MHz
    RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
    RCC->CFGR |= RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9;

    // 5. Set AHB, APB1, APB2 clock dividers
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;     // AHB = SYSCLK
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;    // APB1 = SYSCLK / 2 (max 36 MHz)
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;    // APB2 = SYSCLK

    // 6. Enable PLL and wait for lock
    RCC->CR |= RCC_CR_PLLON;
    while (!(RCC->CR & RCC_CR_PLLRDY));

    // 7. Select PLL as system clock source
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);

    // ==================== GPIO CONFIGURATION ====================

    // 8. Enable GPIO clocks (A, B, C)
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | 
                    RCC_APB2ENR_IOPBEN |
                    RCC_APB2ENR_IOPCEN |
                    RCC_APB2ENR_AFIOEN;

    // 9. Configure PA5 (LED) as output push-pull, 2 MHz
    GPIOA->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);
    GPIOA->CRL |= (0x2 << GPIO_CRL_MODE5); // Output, 2 MHz

    // 10. Configure PA0 as analog input (e.g., for ADC)
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0); // 00 00 = analog input

    // ==================== USART CONFIGURATION ====================

    // 11. Enable USART1 clock (on APB2)
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // 12. Configure PA9 (TX) as alternate function push-pull
    GPIOA->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (0x2 << GPIO_CRH_MODE9);    // Output 2 MHz
    GPIOA->CRH |= (0x2 << GPIO_CRH_CNF9);     // AF Push-pull

    // 13. Configure PA10 (RX) as input floating
    GPIOA->CRH &= ~(GPIO_CRH_MODE10 | GPIO_CRH_CNF10);
    GPIOA->CRH |= (0x1 << GPIO_CRH_CNF10);    // Floating input

    // 14. Set USART1 baud rate to 9600 @ 72MHz
    USART1->BRR = 0x1D4C; // Calculated as 72MHz / 9600

    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;  // Enable TX and RX
    USART1->CR1 |= USART_CR1_UE;                 // Enable USART

    // ==================== ADC CONFIGURATION ====================

    // 15. Enable ADC1 clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 16. Set ADC clock: PCLK2 / 6 = 72 / 6 = 12 MHz (max 14 MHz)
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;

    ADC1->CR2 |= ADC_CR2_ADON;       // Power on ADC
    delay(1000);                     // Short delay
    ADC1->CR2 |= ADC_CR2_ADON;       // Second write to start calibration
    while (!(ADC1->SR & ADC_SR_EOC)); // Wait for end of conversion

    // ==================== TIM2 CONFIGURATION ====================

    // 17. Enable TIM2 clock
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // 18. Configure TIM2 to generate 1 Hz interrupt
    TIM2->PSC = 7200 - 1;    // Prescaler: 72 MHz / 7200 = 10 kHz
    TIM2->ARR = 10000 - 1;   // Auto-reload: 10k -> 1 second
    TIM2->DIER |= TIM_DIER_UIE;  // Enable update interrupt
    TIM2->CR1 |= TIM_CR1_CEN;    // Enable timer

    NVIC_EnableIRQ(TIM2_IRQn);   // Enable interrupt in NVIC

    while (1)
    {
        // Example: Toggle PA5 every loop
        GPIOA->ODR ^= (1 << 5);
        delay(500000);
    }
}

// ==================== TIMER2 ISR ====================
void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF; // Clear update interrupt
        // Do something every second (e.g., toggle LED)
    }
}