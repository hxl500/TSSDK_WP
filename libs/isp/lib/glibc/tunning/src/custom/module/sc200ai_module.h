
{
	.camId = 0,
	.load_sensor = load_sc200ai_sensor_driver,
	.load_actuator = NULL,
	// .load_csi = NULL,
	.load_csi2d = load_sc200ai_csi2d_driver,

	.tuning = {
		{load_sc200ai_normal_tuning, "sc200ai_nor.tun"}, /// mode 0
//		{Load_sc200ai_Dol2_Tuning, "sc200ai_dol2.tun"}, /// mode 0
	},
	.calibration = {load_sc200ai_calibration, "sc200ai.cal"},

},

