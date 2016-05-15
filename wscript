import os
from waflib import TaskGen

# This is for images with no tilemap.
TaskGen.declare_chain(
    name='grit',
    rule='grit ${SRC} -ftb -pS -Mh8 -Mw8 -gB4 -o${TGT[0].bldpath()[:-8]}',
    shell=False,
    ext_in='.png',
    ext_out='.img.bin .pal.bin',
    install_path=None
)


def options(opt):
    opt.load('lua')
    opt.load('compiler_c')


def configure(conf):
    conf.env.LUAC = 'luac5.3'
    conf.load('lua')
    conf.load('compiler_c')
    conf.env.CC = 'arm-none-eabi-gcc'
    conf.env.LINK_CC = 'arm-none-eabi-gcc'

    common_flags = ['-march=armv5te', '-mthumb', '-mthumb-interwork', '-specs=ds_arm9.specs']

    conf.env.CFLAGS = ['-Os'] + common_flags
    conf.env.LINKFLAGS = common_flags
    conf.env.DEVKITPRO = os.getenv('DEVKITPRO', '/usr/local/devkitPRO')

    devkitarm = os.getenv('DEVKITARM', '%s/devkitARM')
    devkitarm_bin = '%s/bin' % (devkitarm)

    conf.find_program('grit', var='GRIT', path_list=[devkitarm_bin])
    conf.find_program('ndstool', var='NDSTOOL', path_list=[devkitarm_bin])
    conf.find_program('objcopy', var='OBJCOPY', path_list=['%s/arm-none-eabi/bin' % devkitarm])


def build(bld):
    # Build Lua
    lua = bld.path.find_dir('3rdparty/lua-5.3.2')

    lua_files = ['lapi.c', 'lcode.c', 'lctype.c', 'ldebug.c', 'ldo.c',
                 'ldump.c', 'lfunc.c', 'lgc.c', 'llex.c', 'lmem.c',
                 'lobject.c', 'lopcodes.c', 'lparser.c', 'lstate.c',
                 'lstring.c', 'ltable.c', 'ltm.c', 'lundump.c', 'lvm.c',
                 'lzio.c', 'lauxlib.c', 'lbaselib.c', 'lbitlib.c',
                 'lcorolib.c', 'ldblib.c', 'linit.c', 'liolib.c',
                 'lmathlib.c', 'loadlib.c', 'loslib.c', 'lstrlib.c',
                 'ltablib.c', 'lutf8lib.c']

    bld.stlib(
        target='lua',
        source=['3rdparty/lua-5.3.2/src/{0}'.format(f) for f in lua_files],
        defines='LUA_COMPAT_5_2')

    # Build game executable
    game_files = ['anim.c', 'debug.c', 'frames.c', 'input.c', 'main.c']

    bld.read_stlib('nds9', paths=['%s/libnds/lib' % bld.env.DEVKITPRO])
    bld.read_stlib('fat', paths=['%s/libnds/lib' % bld.env.DEVKITPRO])

    bld.program(
        source=['src/{0}'.format(f) for f in game_files],
        target='game',
        defines=['ARM9', 'LUA_COMPAT_5_2'],
        includes=['%s/libnds/include' % bld.env.DEVKITPRO,
                  '%s/libnds/include/nds' % bld.env.DEVKITPRO,
                  '3rdparty/lua-5.3.2/src/'],
        use='lua fat nds9',
        stlib='m')

    bld(rule='${OBJCOPY} -O binary ${SRC} ${TGT}',
        source='game',
        target='game.bin')

    bld(rule='${NDSTOOL} -c ${TGT} -9 ${SRC}',
        source='game.bin',
        target='game.nds')

    # Run GRIT on images
    bld(source='gfx/fighters/rasky-walk.png gfx/fighters/rasky-idle.png')

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
        'data/hello.txt',
        'gfx/fighters/rasky-walk.img.bin',
        'gfx/fighters/rasky-walk.pal.bin',
        'gfx/fighters/rasky-idle.img.bin',
        'gfx/fighters/rasky-idle.pal.bin',
        'lua/hello.luac'
    ]

    bld(rule=copy_fat_file, source=data_files, target='game.dat')
