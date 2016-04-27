using System;
using System.Diagnostics;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using Microsoft.SPOT;
using Microsoft.SPOT.Hardware;

namespace OfficeTemperature
{
	public class OfficeTemperatureReporter
	{
		public OfficeTemperatureReporter(OfficeTemperatureConfig config)
		{
			this.config = config;
			temperatureSensor = new Ds18B20(new OneWire(config.TemperatureSensorPort));
		}

		private readonly OfficeTemperatureConfig config;
		private readonly Ds18B20 temperatureSensor;

		public void Start()
		{
			timer = new Timer(TryReportCurrentTemperature, null, TimeSpan.Zero,
				config.TemperatureReportingInterval);
		}

		private Timer timer;

		private void TryReportCurrentTemperature(object state)
		{
			try
			{
				SetStatusLed(false);
				ReportCurrentTemperature();
			}
			catch (Exception exception)
			{
				if (Debugger.IsAttached)
					throw;
				Debug.Print(exception.ToString());
				SetStatusLed(true);
			}
		}

		private void SetStatusLed(bool onOff)
		{
			config.StatusLedPort.Write(onOff);
		}

		private void ReportCurrentTemperature()
		{
			string currentTemperature = temperatureSensor.ConvertAndReadTemperature().ToString();
			string request = BuildGetRequestString(currentTemperature);
			SendGetRequestToServer(request);
		}

		private string BuildGetRequestString(string temperature)
		{
			return "/projekte/ot/temp.php?" +
				"dbg=" + (config.TestMode ? "1" : "0") +
				"&token=" + config.Token +
				"&city=" + config.CityName +
				"&zip=" + config.ZipCode +
				"&country=" + config.CountryCode +
				"&lat=" + config.Latitude +
				"&long=" + config.Longitude +
				"&temp=" + temperature +
				"&type=ard";
		}

		private static void SendGetRequestToServer(string request)
		{
			IPHostEntry host = Dns.GetHostEntry(GolemHostname);
			IPEndPoint endPoint = new IPEndPoint(host.AddressList[0], 80);
			using (var socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp))
			{
				socket.Connect(endPoint);
				using (var networkStream = new NetworkStream(socket, true))
				{
					string command = "GET " + request + " HTTP/1.1\r\nHost: " + endPoint.Address + "\r\n\r\n";
					var bytes = Encoding.UTF8.GetBytes(command);
					networkStream.Write(bytes, 0, bytes.Length);
					ReadServerResponseAndCheckStatus(networkStream);
				}
			}
		}

		private const string GolemHostname = "www.golem.de";

		private static void ReadServerResponseAndCheckStatus(Stream stream)
		{
			string responseMessage = "";
			using (var reader = new StreamReader(stream))
			{
				while (true)
				{
					string currentLine = reader.ReadLine();
					if (currentLine == HttpStatusOk)
						return;
					if (currentLine == EndOfResponseMessage)
						break;
					responseMessage += currentLine + "\r\n";
				}
			}
			throw new SendTemperatureToServerFailed(responseMessage);
		}

		private const string EndOfResponseMessage = "0";
		private const string HttpStatusOk = "HTTP/1.1 200 OK";

		private class SendTemperatureToServerFailed : Exception
		{
			public SendTemperatureToServerFailed(string message) : base(message) {}
		}
	}
}