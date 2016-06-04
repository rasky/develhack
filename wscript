# -*- python -*-

import os
from itertools import chain


def parse_grit(grit):
    """
    Parse a .grit file and return a tuple of the options to the grit command
    line, the inputs to be used (all the .png files in the same directory as
    the .grit file) and the outputs that will be produced (calculated based
    on the options found in the grit file).
    """
    options = []
    with open(grit.abspath(), 'r') as optfile:
        for line in optfile:
            line = line.strip().split('#', 1)[0]
            if not line:
                continue
            options.extend(line.split(' '))

    inputs = grit.parent.ant_glob('*.png')

    extensions = []
    shared_extensions = []

    shared = None
    try:
        shared = options[options.index('-O') + 1]
    except ValueError:
        for opt in options:
            if opt.startswith('-O'):
                shared = opt[2:]

    if not shared:
        shared = inputs[0].change_ext('')

    if '-fh!' not in options:
        extensions.append('.h')

    if '-g!' not in options:
        if '-gS' in options:
            shared_extensions.append('.img.bin')
        else:
            extensions.append('.img.bin')

    if '-p!' not in options:
        if '-pS' in options:
            shared_extensions.append('.pal.bin')
        else:
            extensions.append('.pal.bin')

    if '-m' in options:
        extensions.append('.map.bin')

        if any(opt.startswith('-Mh') or opt.startswith('-Mw') for opt in options):
            extensions.append('.meta.bin')

    out = grit.get_bld().parent

    outputs = list(chain(
        (png.change_ext(ext) for ext in extensions for png in inputs),
        (out.make_node('{0}{1}'.format(shared, ext)) for ext in shared_extensions)
    ))

    return options, inputs, outputs


def options(opt):
    opt.load('lua')
    opt.load('compiler_c')

    opt.add_option('-d', '--debug-on-screen', action='store_true', default=False)
    opt.add_option('-u', '--with-unit-tests', action='store_true', default=False)


def configure(conf):
    conf.env.LUAC = 'luac5.3'
    conf.load('lua')
    conf.load('compiler_c')
    conf.env.CC = 'arm-none-eabi-gcc'
    conf.env.LINK_CC = 'arm-none-eabi-gcc'

    # Common
    conf.env.CFLAGS = ['-Os', '-Wall', '-Werror', '-mthumb', '-mthumb-interwork']
    conf.env.DEVKITPRO = os.getenv('DEVKITPRO', '/usr/local/devkitPRO')

    # ARM9
    conf.env.CFLAGS_ARM9 = conf.env.CFLAGS + ['-specs=ds_arm9.specs', '-march=armv5te']
    conf.env.DEFINES_ARM9 = ['ARM9']
    conf.env.LINKFLAGS_ARM9 = conf.env.CFLAGS_ARM9 + ['-Wl,-Map,game.arm9.map']

    # ARM7
    conf.env.CFLAGS_ARM7 = conf.env.CFLAGS + ['-specs=ds_arm7.specs', '-mcpu=arm7tdmi']
    conf.env.DEFINES_ARM7 = ['ARM7']
    conf.env.LINKFLAGS_ARM7 = conf.env.CFLAGS_ARM7 + ['-Wl,-Map,game.arm7.map']

    if conf.options.debug_on_screen:
        conf.env.DEFINES += ['DEBUG_ON_SECONDARY_SCREEN']

    if conf.options.with_unit_tests:
        conf.env.DEFINES += ['WITH_UNIT_TESTS']

    devkitarm = os.getenv('DEVKITARM', '%s/devkitARM')
    devkitarm_bin = '%s/bin' % (devkitarm)

    conf.find_program('grit', var='GRIT', path_list=[devkitarm_bin])
    conf.find_program('ndstool', var='NDSTOOL', path_list=[devkitarm_bin])
    conf.find_program('objcopy', var='OBJCOPY', path_list=['%s/arm-none-eabi/bin' % devkitarm])


def build(bld):
    # bld.stlib(
    #     target='lua',
    #     source=bld.path.ant_glob("3rdparty/lua-5.3.2/src/*.c"),
    #     defines='LUA_COMPAT_5_2',
    #     use='ARM9')

    # Build game executable
    libxm_path = '3rdparty/libxm7-1.06'

    bld.read_stlib('fat', paths=['%s/libnds/lib' % bld.env.DEVKITPRO])
    bld.read_stlib('nds7', paths=['%s/libnds/lib' % bld.env.DEVKITPRO])
    bld.read_stlib('nds9', paths=['%s/libnds/lib' % bld.env.DEVKITPRO])
    bld.read_stlib('xm77', paths=[libxm_path], export_includes=[libxm_path])
    bld.read_stlib('xm79', paths=[libxm_path], export_includes=[libxm_path])

    # ARM9
    bld.program(
        source=bld.path.ant_glob("src/*.c"),
        target='game.arm9',
        defines=['LUA_COMPAT_5_2'],
        includes=['%s/libnds/include' % bld.env.DEVKITPRO,
                  '%s/libnds/include/nds' % bld.env.DEVKITPRO,
                  '3rdparty/lua-5.3.2/src/'],
        use='ARM9 fat xm79 nds9',
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
        use='ARM7 xm77 nds7')

    bld(rule='${OBJCOPY} -O binary ${SRC} ${TGT}',
        source='game.arm7',
        target='game.arm7.bin')

    # Final game binary
    bld(rule='${NDSTOOL} -c ${TGT} -9 ${SRC[0]} -7 ${SRC[1]}',
        source='game.arm9.bin game.arm7.bin',
        target='game.nds')

    # Run GRIT on images
    data_files = []

    for gritfile in bld.path.ant_glob('gfx/**/*.grit'):
        options, inputs, outputs = parse_grit(gritfile)
        input_paths = " ".join(i.abspath() for i in inputs)
        bld(rule="${{GRIT}} {0} {1}".format(input_paths, " ".join(options)),
            source=inputs,
            target=outputs,
            cwd=outputs[0].parent.abspath())
        data_files.extend(outputs)

    bld(source=bld.path.ant_glob('lua/*.lua'))

    data_files += [i.change_ext('.luac') for i in bld.path.ant_glob('lua/*.lua')]
    data_files += list(bld.path.ant_glob('sfx/**'))

    # Build FAT data image
    def copy_fat_file(task):
        tgt = task.outputs[0].abspath()

        if not os.path.exists(tgt):
            # 10 is roughly the size, in megs, of the resulting disk image
            ret = task.exec_command('mformat -C -t 10 -h 64 -s 32 -H 32 -i %s' % (tgt))
            if ret != 0:
                return ret

        for src in task.inputs:
            ret = task.exec_command('mcopy -o -i %s %s ::' % (tgt, src.abspath()))
            if ret != 0:
                return ret

        return task.exec_command('mdir -i %s' % (tgt,))

    bld(rule=copy_fat_file, source=data_files, target='game.dat')

    # Collect resources in a single folder
    def cp(task):
        tgt = task.outputs[0].parent
        tgt.mkdir()
        for src in task.inputs:
            ret = task.exec_command('cp {0} {1}'.format(src.abspath(), tgt.abspath()))
        return ret

    bld(rule=cp, source=data_files,
        target=['resources/{0}'.format(os.path.basename(f.abspath())) for f in data_files])
