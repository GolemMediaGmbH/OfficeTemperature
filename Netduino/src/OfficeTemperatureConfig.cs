using System;
using Microsoft.SPOT.Hardware;

namespace OfficeTemperature
{
	public class OfficeTemperatureConfig
	{
		public OutputPort TemperatureSensorPort { get; set; }
		public OutputPort StatusLedPort { get; set; }
		public TimeSpan TemperatureReportingInterval { get; set; }
		public bool TestMode { get; set; }
		public string Token { get; set; }
		public string CountryCode { get; set; }
		public string CityName { get; set; }
		public string ZipCode { get; set; }
		public string Latitude { get; set; }
		public string Longitude { get; set; }
	}
}