package cbd_plugin

import (
	"fmt"
	"strings"

	"android/soong/android"
	"android/soong/cc"
)

func init() {
	android.RegisterModuleType("cbd_enabled_preset", cbdFactory)
}

func cbdFactory() android.Module {
	module := cc.DefaultsFactory()
	android.AddLoadHook(module, cbdSetEnabled)

	return module
}

func cbdSetEnabled(ctx android.LoadHookContext) {
	type props struct {
		Enabled *bool
		Cflags []string
	}

	p := &props{}
	p.Enabled = new(bool)
	*p.Enabled, p.Cflags = cbdCheckVendorConfig(ctx)

	ctx.AppendProperties(p)
}

func cbdCheckVendorConfig(ctx android.BaseContext) (bool, []string) {
	cmd := "cbd"
	versionKey := "version"
	protocolKey := "protocol"
	flagsKey := "flags"
	var cflags []string

	if !ctx.AConfig().VendorConfig(cmd).IsSet(versionKey) ||
			!ctx.AConfig().VendorConfig(cmd).IsSet(protocolKey) {
		return false, cflags
	}

	version := ctx.AConfig().VendorConfig(cmd).String(versionKey)
	protocol := ctx.AConfig().VendorConfig(cmd).String(protocolKey)
	enableModuleName := fmt.Sprintf("%s_%s.%s.enabled", cmd, version, protocol)

	if enableModuleName == ctx.ModuleName() {
		if ctx.AConfig().VendorConfig(cmd).IsSet(flagsKey) {
			flags := ctx.AConfig().VendorConfig(cmd).String(flagsKey)
			flaglist := strings.Split(flags, " ")

			for _, flag := range flaglist {
				cflags = append(cflags, "-D" + strings.TrimSpace(flag))
			}
		}

		return true, cflags
	}

	return false, cflags
}
