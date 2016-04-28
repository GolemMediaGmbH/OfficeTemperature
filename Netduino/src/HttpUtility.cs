using System.Text;
using System.Text.RegularExpressions;

namespace OfficeTemperature
{
	internal static class HttpUtility
	{
		public static string BuildGetRequestUri(string parameterName, string parameterValue)
		{
			if (parameterValue == null)
				return string.Empty;
			return "&" + parameterName + "=" + parameterValue;
		}

		public static string BuildGetRequest(string requestUri, string host)
		{
			return "GET " + requestUri + " HTTP/1.1\r\nHost: " + host + "\r\n\r\n";
		}

		public static string UrlEncode(string input)
		{
			var encodedInput = new StringBuilder(input);
			var regex = new Regex(CharactersToEncode);
			foreach (Match match in regex.Matches(input))
			{
				string oldValue = input[match.Index].ToString();
				string encodedValue = UrlEncodedMatch(match);
				encodedInput.Replace(oldValue, encodedValue, match.Index, oldValue.Length);
			}
			return encodedInput.ToString();
		}

		private static readonly string CharactersToEncode = "[^0-9a-zA-Z" + Regex.Escape("-_.!~*'()") + "]";

		private static string UrlEncodedMatch(Match match)
		{
			if (match.Value == " ")
				return "+";
			var byteValues = Encoding.UTF8.GetBytes(match.Value);
			var hexString = "";
			foreach (var byteValue in byteValues)
				hexString += "%" + byteValue.ToString("X2");
			return hexString;
		}
	}
}