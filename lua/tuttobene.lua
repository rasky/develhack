function init()
	--        name         width height floory fighter start X   fighter limits
	set_stage("tuttobene", 1024, 512, 500, { 412, 612 }, { 50, 970 })

	-- set_layer(0, "lounge", "cielo.map.bin", 2, Background.text_8_bpp_512x512, 0, 0, 512, 512, false)
	set_layer(2, "tuttobene", "tuttobene0.map.bin", 1, Background.exrot_1024x1024, 0, 0, 1024, 512, false)
	set_layer(3, "tuttobene", "tuttobene1.map.bin", 0, Background.exrot_1024x1024, 0, 0, 1024, 512, false)
end

function update(stage)
end
