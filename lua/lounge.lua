function init()
	--        name      width height floory fighter start X   fighter limits
	set_stage("lounge", 1024, 512, 500, { 412, 612 }, { 50, 970 })

	set_layer(0, "lounge", "cielo.map.bin", 2, Background.text_8_bpp_512x512, 0, 0, 512, 512, false)
	set_layer(2, "lounge", "lounge-00.map.bin", 1, Background.exrot_1024x1024, 0, 0, 1024, 512, false)
	set_layer(3, "lounge", "lounge-01.map.bin", 0, Background.exrot_1024x1024, 0, 0, 1024, 512, false)

	stage = {
		foozball=gen_sprite[2]
	}

	-- TODO
	-- foozball = gen_sprite(2)
	-- set_sprite(stage.foozball[1], x, y, priority, palette_alpha, size, format, offset, affineIndex, double, hide, hflip, vflip, mosaic)
	-- set_sprite(stage.foozball[2], x, y, priority, palette_alpha, size, format, offset, affineIndex, double, hide, hflip, vflip, mosaic)
	return stage
end

function update(stage)
	-- TODO
	-- set_sprite(stage.pigeon, x, y, priority, pal_alpha, size, format, offset, affineIndex, double, hide, hflip, vflip, mosaic)
end
