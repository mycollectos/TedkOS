#include <stdint.h>
#include <stddef.h>
#include <inc/keyboard.h>
#include <inc/d2d/to_term.h>

#define KB_PORT 0x60
#define KB_INT_NUM 0x21
#define KB_IRQ_NUM 1
#define KB_ID 1
#define KD_POLICY 0


/*
* See http://wiki.osdev.org/PS/2_Keyboard
*/
uint32_t KBascii[128] =
{
    /* keycode here is all pressed keycode */
    0,/* unused,nothing happened */
    KKC_ESC,/* esc */
	0x02,/* 1 */
    0x03,/* 2 */
    0x04,/* 3 */
    0x05,/* 4 */
    0x06,/* 5 */
    0x07,/* 6 */
    0x08,/* 7 */
    0x09,/* 8 */
    0x0A,/* 9 */
    0x0B,/* 0 */
	0x0C,/* - */
    0x0D,/* = */
	KKC_BACKSPACE,/* Backspace */
	KKC_TAB,/* Tab */
	0x10,/* Q (default: q)*/
    0x11,/* W */
    0x12,/* E */
    0x13,/* R */
	0x14,/* T */
    0x15,/* Y */
    0x16,/* U */
    0x17,/* I */
    0x18,/* O */
    0x19,/* P */
    0x1A,/* [ */
    0x1B,/* ] */
	KKC_ENTER,	/* Enter key */
    KKC_CTRL,	/* left Control */
	0x1E,/* A */
    0x1F,/* S */
    0x20,/* D */
    0x21,/* F */
    0x22,/* G */
    0x23,/* H */
    'j',/* J */
    'k',
    'l',
    ';',	/* 39 */
	'\'', '`',
	KKC_SHIFT,	/* Left shift */
	'\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
	'm', ',', '.', '/',
	KKC_SHIFT,	/* Right shift */
	'*',
    KKC_ALT,	/* Alt */
	' ',/* Space */
    KKC_CAPSLOCK,	/* Caps lock */
    0,
	/* 59 - F1 key ... > */
    0, 0, 0, 0, 0, 0, 0, 0, 0,
	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    KKC_HOME,	/* Home key */
    KKC_UP,	/* Up Arrow */
    KKC_PAGEUP,	/* Page Up */
	'-',
    KKC_LEFT,	/* Left Arrow */
    0,
    KKC_RIGHT,	/* Right Arrow */
	'+',
    KKC_END,	/* 79 - End key*/
    KKC_DOWN,	/* Down Arrow */
    KKC_PAGEDOWN,	/* Page Down */
    KKC_INSERT,	/* Insert Key */
    KKC_DELETE,	/* Delete Key */
    0, 0, 0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

int kb_handler(int irq, unsigned int saved_reg);

DEFINE_DRIVER_INIT(kb) {
	// bind handler to pic
	bind_irq(KB_IRQ_NUM,KB_ID,kb_handler,KD_POLICY);
	return;
}

DEFINE_DRIVER_REMOVE(kb) {
	//rm handler from pic
	unbind_irq(KB_IRQ_NUM,KB_ID);
    return;
}

/* keyboard_handler
 * description:
 *		Handle keyboard interrupts
 * input:
 *		int irq, pt_reg* saved_reg(not used)
 * output,return:
 *		currently the value is ignored, 0 for success.
 *      Look at <inc/error.h> for more error codes.
 * side effect:
 *		changes keyboard buffer (if we have a buffer)
 *      print keyboard character to terminal
 */
int kb_handler(int irq, unsigned int saved_reg){
	uint8_t keyboard_scancode;

 	keyboard_scancode = inb(KB_PORT);//read the input

    //!!! WARNING: We should not use "keyboard_scancode & 0x80" anymore
    //!!!   Because that filters out the events for release keys
    //!!!   BUT we need to know when keys like SHIFT, ALT are released

 	if (!(keyboard_scancode)) {
 		uint32_t kernel_keycode = KBascii[keyboard_scancode];
 		kb_to_term(kernel_keycode);
 	}

    return 0;
}
