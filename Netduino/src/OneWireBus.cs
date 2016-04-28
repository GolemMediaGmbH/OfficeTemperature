using System.Collections;
using Microsoft.SPOT.Hardware;

namespace OfficeTemperature
{
	/// <summary>
	/// https://www.ghielectronics.com/community/codeshare/entry/438
	/// </summary>
	internal static class OneWireBus
	{
		public enum Family : byte
		{
			Unknown = 0x00,
			DS18S20 = 0x10,
			DS18B20 = 0x28
		}

		public static Device[] Scan(OneWire ow, params Family[] includeFamilies)
		{
			var list = new ArrayList();
			var all = false;
			var devs = ow.FindAllDevices();
			if (includeFamilies != null)
			{
				foreach (var f in includeFamilies)
				{
					if (f == Family.Unknown)
						all = true;
				}
			}
			foreach (byte[] da in devs)
			{
				if (includeFamilies == null || includeFamilies.Length == 0 || all)
				{
					list.Add(new Device(da));
				}
				else
				{
					foreach (var f in includeFamilies)
					{
						if (da[0] == (byte)f)
							list.Add(new Device(da));
					}
				}
			}
			return (Device[])list.ToArray(typeof(Device));
		}

		public static Device[] Scan(Cpu.Pin pin, params Family[] includeFamilies)
		{
			using (var op = new OutputPort(pin, false))
				return Scan(new OneWire(op), includeFamilies);
		}

		public class Device
		{
			internal Device(byte[] addr)
			{
				Address = addr;
				switch (addr[0])
				{
				case 0x10:
					Family = Family.DS18S20;
					break;
				case 0x28:
					Family = Family.DS18B20;
					break;
				default:
					Family = Family.Unknown;
					break;
				}
			}

			public byte[] Address { get; private set; }

			public Family Family { get; private set; }
		}
	}
}