
struct gpio_regs {
	unsigned int MODER;
	unsigned int OTYPER;
	unsigned int OSPEEDR;
	unsigned int PUPDR;
	unsigned int IDR;
	unsigned int ODR;
	unsigned int BSRR;
	unsigned int LCKR;
	unsigned int AFRL;
	unsigned int AFRH;
	unsigned int BRR;;
};

struct rcc_regs {
	unsigned int CR;
	unsigned int CFGR;
	unsigned int CIR;
	unsigned int APB2RSTR;
	unsigned int APB1RSTR;
	unsigned int AHBENR;
	unsigned int APB2ENR;
	unsigned int APB1ENR;
	unsigned int BDCR;
	unsigned int CSR;
	unsigned int AHBRSTR;
	unsigned int CFGR2;
	unsigned int CFGR3;
	unsigned int CR2;
};


#define RCC_CFGR_SW_OFFSET 0
#define RCC_CFGR_SWS_OFFSET 2
#define RCC_CFGR_HPRE_OFFSET 4
#define RCC_CFGR_PPRE_OFFSET 8
#define RCC_CFGR_ADCPRE_OFFSET 14
#define RCC_CFGR_PLLSRC_OFFSET 15
#define RCC_CFGR_PLLXTPRE_OFFSET 17
#define RCC_CFGR_PLLMUL_OFFSET 18
#define RCC_CFGR_MCO_OFFSET 24
#define RCC_CFGR_MCOPRE_OFFSET 28
#define RCC_CFGR_PLLNODIV_OFFSET 31

#define RCC_CFGR2_PREDIV_OFFSET 0

extern volatile struct gpio_regs GPIOB;
extern volatile struct gpio_regs GPIOE;
extern volatile struct rcc_regs RCC;
