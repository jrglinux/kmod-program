#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xd9726f80, "module_layout" },
	{ 0x7eb2fe55, "vinput_unregister" },
	{ 0x8c787952, "vinput_register" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x82c89bb8, "input_register_device" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x1a1edd5a, "_dev_err" },
	{ 0xcd13c4db, "input_event" },
	{ 0xc6810313, "_dev_info" },
	{ 0x50d4de4, "pv_ops" },
	{ 0xba8fbd64, "_raw_spin_lock" },
	{ 0x3854774b, "kstrtoll" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "vinput");


MODULE_INFO(srcversion, "76EDEC8020CEDCDCB0B45FA");
