#ifndef DECKARD_DKW_USB_PAYLOAD_H
#define DECKARD_DKW_USB_PAYLOAD_H

#define DKW_USB_PAYLOAD_TARGET_ADDR ((volatile unsigned int*)0x80A01F80)
#define DKW_USB_PAYLOAD_TARGET_PPC 0x00A01F80
#define DKW_USB_PAYLOAD_WORD_COUNT 0x19A0

extern const unsigned int g_dkw_usb_payload_words[DKW_USB_PAYLOAD_WORD_COUNT];

#endif /* DECKARD_DKW_USB_PAYLOAD_H */
