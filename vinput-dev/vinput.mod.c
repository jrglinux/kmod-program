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
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x36c11c94, "kmalloc_caches" },
	{ 0x3854774b, "kstrtoll" },
	{ 0x837b7b09, "__dynamic_pr_debug" },
	{ 0x4102c7cb, "__register_chrdev" },
	{ 0x21271fd0, "copy_user_enhanced_fast_string" },
	{ 0x50d4de4, "pv_ops" },
	{ 0xac133870, "_dev_warn" },
	{ 0x96719680, "device_register" },
	{ 0xc5850110, "printk" },
	{ 0xc5645b17, "class_unregister" },
	{ 0x1f199d24, "copy_user_generic_string" },
	{ 0x5a921311, "strncmp" },
	{ 0xdd2f8305, "__class_register" },
	{ 0xecdcabd2, "copy_user_generic_unrolled" },
	{ 0x50f6845f, "module_put" },
	{ 0xc6810313, "_dev_info" },
	{ 0xa916b694, "strnlen" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0x69ecc112, "kmem_cache_alloc_trace" },
	{ 0xba8fbd64, "_raw_spin_lock" },
	{ 0x37a0cba, "kfree" },
	{ 0x13540d7d, "input_unregister_device" },
	{ 0xed208a09, "device_unregister" },
	{ 0x7375239f, "dev_set_name" },
	{ 0xe271efb4, "try_module_get" },
	{ 0xbbfa8276, "input_allocate_device" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "0047CD57A986CBAEA7B5E08");
