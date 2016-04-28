using System;
using System.Threading;
using Microsoft.SPOT.Hardware;
using SecretLabs.NETMF.Hardware.NetduinoPlus;

namespace OfficeTemperature.NetduinoPlus
{
	/// <summary>
	/// For the Netduino Plus Gen 1 it is required to flash a custom firmware to get support for
	/// the 1-Wire bus system protocol used by the DS18B20 sensor.
	/// Tested with this buid:
	/// http://forums.netduino.com/index.php?/topic/6968-hardware-watchdog-on-netduino-plus/page-2#entry47334
	/// </summary>
	public class Program
	{
		public static void Main()
		{
			var config = new OfficeTemperatureConfig
			{
				TemperatureSensorPort = new OutputPort(Pins.GPIO_PIN_D2, false),
				StatusLedPort = new OutputPort(Pins.ONBOARD_LED, false),
				TemperatureReportingInterval = new TimeSpan(0, 0, 15, 00),
				TestMode = true,
				Token = "00000000000000000000000000000000",
				CountryCode = "DE",
				CityName = "Berlin",
				ZipCode = "10969",
				Latitude = "52.50",
				Longitude = "13.41"
			};
			temperatureReporter = new OfficeTemperatureReporter(config);
			temperatureReporter.Start();
			Thread.Sleep(Timeout.Infinite);
		}

		private static OfficeTemperatureReporter temperatureReporter;
	}
}