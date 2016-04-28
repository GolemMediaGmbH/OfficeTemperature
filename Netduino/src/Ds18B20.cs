using System;
using System.Threading;
using Microsoft.SPOT.Hardware;

namespace OfficeTemperature
{
	/// <summary>
	/// https://www.ghielectronics.com/community/codeshare/entry/438
	/// </summary>
	internal class Ds18B20 : IDisposable
	{
		private readonly OneWireBus.Device m_dev;
		private readonly OutputPort m_op;
		private readonly OneWire m_ow;

		public Ds18B20(Cpu.Pin pin) : this(pin, null) {}

		public Ds18B20(Cpu.Pin pin, OneWireBus.Device dev)
		{
			m_op = new OutputPort(pin, false);
			m_ow = new OneWire(m_op);
			if (dev == null)
			{
				var devs = OneWireBus.Scan(m_ow, OneWireBus.Family.DS18B20);
				if (devs == null || devs.Length < 1)
					throw new InvalidOperationException("No DS18B20 devices found on OneWire bus");
				dev = devs[0];
			}
			m_dev = dev;
		}

		public Ds18B20(OneWire ow) : this(ow, null) {}

		public Ds18B20(OneWire ow, OneWireBus.Device dev)
		{
			m_ow = ow;
			if (dev == null)
			{
				var devs = OneWireBus.Scan(ow, OneWireBus.Family.DS18B20);
				if (devs == null || devs.Length < 1)
					throw new InvalidOperationException("No DS18B20 devices found on OneWire bus");
				dev = devs[0];
			}
			m_dev = dev;
		}

		public void Dispose()
		{
			if (m_op != null)
				m_op.Dispose();
		}

		public float ConvertAndReadTemperature()
		{
			var data = 0L;

			// if reset finds no devices, just return 0
			if (m_ow.TouchReset() == 0)
				return 0;

			// address the device
			m_ow.WriteByte(Command.MatchROM);
			WriteBytes(m_dev.Address);

			// tell the device to start temp conversion
			m_ow.WriteByte(Command.StartTemperatureConversion);

			// wait for as long as it takes to do the temp conversion,
			// data sheet says ~750ms
			while (m_ow.ReadByte() == 0)
				Thread.Sleep(1);

			// reset the bus
			m_ow.TouchReset();

			// address the device
			m_ow.WriteByte(Command.MatchROM);
			WriteBytes(m_dev.Address);

			// read the data from the sensor
			m_ow.WriteByte(Command.ReadScratchPad);

			// read the two bytes of data
			data = m_ow.ReadByte(); // LSB
			data |= (ushort)(m_ow.ReadByte() << 8); // MSB

			// reset the bus, we don't want more data than that
			m_ow.TouchReset();

			// returns C
			// F would be:  (float)((1.80 * (data / 16.00)) + 32.00);
			return data / 16f;
		}

		public void StartConversion()
		{
			// if reset finds no devices, just return 0
			if (m_ow.TouchReset() == 0)
				return;

			// address the device
			m_ow.WriteByte(Command.MatchROM);
			WriteBytes(m_dev.Address);

			// tell the device to start temp conversion
			m_ow.WriteByte(Command.StartTemperatureConversion);
		}

		public float ReadTemperature()
		{
			var data = 0L;

			// reset the bus
			m_ow.TouchReset();

			// address the device
			m_ow.WriteByte(Command.MatchROM);
			WriteBytes(m_dev.Address);

			// read the data from the sensor
			m_ow.WriteByte(Command.ReadScratchPad);

			// read the two bytes of data
			data = m_ow.ReadByte(); // LSB
			data |= (ushort)(m_ow.ReadByte() << 8); // MSB

			// reset the bus, we don't want more data than that
			m_ow.TouchReset();

			// returns C
			// F would be:  (float)((1.80 * (data / 16.00)) + 32.00);
			return data / 16f;
		}

		public static float ToFahrenheit(float tempC)
		{
			return 9f / 5f * tempC + 32f;
		}

		private void WriteBytes(byte[] data)
		{
			for (var i = 0; i < data.Length; i++)
				m_ow.WriteByte(data[i]);
		}

		private static class Command
		{
			public const byte SearchROM = 0xF0;
			public const byte ReadROM = 0x33;
			public const byte MatchROM = 0x55;
			public const byte SkipROM = 0xCC;
			public const byte AlarmSearch = 0xEC;
			public const byte StartTemperatureConversion = 0x44;
			public const byte ReadScratchPad = 0xBE;
			public const byte WriteScratchPad = 0x4E;
			public const byte CopySratchPad = 0x48;
			public const byte RecallEEPROM = 0xB8;
			public const byte ReadPowerSupply = 0xB4;
		}
	}
}