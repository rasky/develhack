# -*- python -*-

import os

from waflib import TaskGen


def grit_rule(task):
    for src in task.inputs:
        output_dir = task.outputs[0].parent.abspath()
        command = ['grit'] + [src.abspath()] + list(task.generator.options)
        task.exec_command(' '.join(command), cwd=output_dir)


def grit_decider(task, node):
    if '-ftb' not in task.options:
        raise Exception('Only -ftb is supported at the moment')

    extensions = ['.h', '.img.bin', '.pal.bin']

    if '-m' in task.options:
        extensions.append('.map.bin')

        if any(opt.startswith('-Mh') or opt.startswith('-Mw') for opt in task.options):
            extensions.append('.meta.bin')

    return extensions

TaskGen.declare_chain(
    name='grit',
    rule=grit_rule,
    decider=grit_decider,
    ext_in='.png'
)


def options(opt):
    opt.load('lua')
    opt.load('compiler_c')

    opt.add_option('-d', '--debug-on-screen', action='store_true', default=False)


def configure(conf):
    conf.env.LUAC = 'luac5.3'
    conf.load('lua')
    conf.load('compiler_c')
    conf.env.CC = 'arm-none-eabi-gcc'
    conf.env.LINK_CC = 'arm-none-eabi-gcc'

    # Common
    conf.env.CFLAGS = ['-Os', '-Wall', '-march=armv5te', '-mthumb', '-mthumb-interwork']
    conf.env.DEVKITPRO = os.getenv('DEVKITPRO', '/usr/local/devkitPRO')

    # ARM9
    conf.env.CFLAGS_ARM9 = conf.env.CFLAGS + ['-specs=ds_arm9.specs']
    conf.env.DEFINES_ARM9 = ['ARM9']
    conf.env.LINKFLAGS_ARM9 = conf.env.CFLAGS_ARM9 + ['-Wl,-Map,game.arm9.map']

    # ARM7
    conf.env.CFLAGS_ARM7 = conf.env.CFLAGS + ['-specs=ds_arm7.specs']
    conf.env.DEFINES_ARM7 = ['ARM7']
    conf.env.LINKFLAGS_ARM7 = conf.env.CFLAGS_ARM7 + ['-Wl,-Map,game.arm7.map']

    if conf.options.debug_on_screen:
        conf.env.DEFINES += ['DEBUG_ON_SECONDARY_SCREEN']

    devkitarm = os.getenv('DEVKITARM', '%s/devkitARM')
    devkitarm_bin = '%s/bin' % (devkitarm)

    conf.find_program('grit', var='GRIT', path_list=[devkitarm_bin])
    conf.find_program('ndstool', var='NDSTOOL', path_list=[devkitarm_bin])
    conf.find_program('objcopy', var='OBJCOPY', path_list=['%s/arm-none-eabi/bin' % devkitarm])


def build(bld):
    bld.stlib(
        target='lua',
        source=bld.path.ant_glob("3rdparty/lua-5.3.2/src/*.c"),
        defines='LUA_COMPAT_5_2',
        use='ARM9')

    # Build game executable
    bld.read_stlib('fat', paths=['%s/libnds/lib' % bld.env.DEVKITPRO])
    bld.read_stlib('nds7', paths=['%s/libnds/lib' % bld.env.DEVKITPRO])
    bld.read_stlib('nds9', paths=['%s/libnds/lib' % bld.env.DEVKITPRO])

    # ARM9
    bld.program(
        source=bld.path.ant_glob("src/*.c"),
        target='game.arm9',
        defines=['LUA_COMPAT_5_2'],
        includes=['%s/libnds/include' % bld.env.DEVKITPRO,
                  '%s/libnds/include/nds' % bld.env.DEVKITPRO,
                  '3rdparty/lua-5.3.2/src/'],
        use='ARM9 lua fat nds9',
        stlib='m')

    bld(rule='${OBJCOPY} -O binary ${SRC} ${TGT}',
        source='game.arm9',
        target='game.arm9.bin')

    # ARM7
    bld.program(
        source=bld.path.ant_glob("src/debug.c src/arm7/*.c"),
        target='game.arm7',
        includes=[
            '%s/libnds/include' % bld.env.DEVKITPRO,
            '%s/libnds/include/nds' % bld.env.DEVKITPRO,
        ],
        use='ARM7 nds7')

    bld(rule='${OBJCOPY} -O binary ${SRC} ${TGT}',
        source='game.arm7',
        target='game.arm7.bin')

    # Final game binary
    bld(rule='${NDSTOOL} -c ${TGT} -9 ${SRC[0]}',
        source='game.arm9.bin game.arm7.bin',
        target='game.nds')

    # Run GRIT on images
    fighters = ['rasky-walk', 'rasky-idle']

    bld(options=('-ftb', '-pS', '-Mh8', '-Mw8', '-gB4'),
        source=['gfx/fighters/{0}.png'.format(f) for f in fighters])

    # Build FAT data image
    def copy_fat_file(task):
        tgt = task.outputs[0].abspath()

        if not os.path.exists(tgt):
            task.exec_command('mformat -C -t 1 -h 1 -s 512 -i %s' % (tgt))

        for src in task.inputs:
            task.exec_command('mcopy -o -i %s %s ::' % (tgt, src.abspath()))

        return task.exec_command('mdir -i %s' % (tgt,))

    bld(source='lua/hello.lua')

    data_files = [
        'gfx/fighters/rasky-walk.img.bin',
        'gfx/fighters/rasky-walk.pal.bin',
        'gfx/fighters/rasky-idle.img.bin',
        'gfx/fighters/rasky-idle.pal.bin',
        'lua/hello.luac'
    ]

    bld(rule=copy_fat_file, source=data_files, target='game.dat')

    # Collect resources in a single folder
    def cp(task):
        tgt = task.outputs[0].abspath()
        if not os.path.exists(tgt):
            os.makedirs(tgt)
        for src in task.inputs:
            ret = task.exec_command('cp {0} {1}'.format(src.abspath(), tgt))
        return ret

    bld(rule=cp, source=data_files, target='resources')
