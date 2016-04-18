#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <streambuf>

int main()
{

	std::ifstream input_zip("./input.zip", std::ifstream::in | std::ifstream::binary);
	std::ofstream output_js("./output.js", std::ifstream::out | std::ifstream::trunc);

	std::string header;

	std::vector<unsigned char> str((std::istreambuf_iterator<char>(input_zip)),
		std::istreambuf_iterator<char>());

	std::stringstream output_buffer;

	header = "var v_plus_client_data_array = new Uint8Array([\n";
	output_buffer << header;

	bool first = true;

	output_buffer << std::hex;

	for (const auto& c : str)
	{
		if (first)
		{
			first = false;
			output_buffer << "0x" << (int)c;
		}
		else
		{
			output_buffer << ",0x" << (int)c;
		}
	}

	header = "\n]);\n";
	output_buffer << header;

	output_js << output_buffer.str();

	output_js.close();
	input_zip.close();

	return 0;
}

/*HTML FILE:
<html lang="en">
	<head>
		<meta http-equiv="content-type" content="text/html; charset=utf-8">
		<title>Grand Theft Auto V+ Multiplayer Mod - Download Page</title>
		<meta name="description" content="Grand Theft Auto V+ Mod (Multiplayer)">
		<meta name="keywords" content="GTAV,GTA,V,GTA V,V+,V Plus,GTA V+,V+ Mod">
		<meta name="author" content="V+ Team">
	</head>
	<body>
		<div>
		<a id="download" href="#">Start Download (Version 70)</a><br/>
		SHA-256: -<br/>
		MD5: -<br/>
		CRC32: -<br/>
		</div>
	</body>
	<script>
		
		function startDownload()
		{
			window.alert("Downloading");
			var a = window.document.createElement('a');
			a.href = window.URL.createObjectURL(new Blob([v_plus_client_data_array], { type: 'application/octet-stream' }));
			a.download = "v-plus.zip";
			document.body.appendChild(a);
			a.click();
			document.body.removeChild(a);
		};
				
		window.onload = function()
		{
			window.alert("Loaded");
			document.getElementById("download").addEventListener("click", startDownload, false);
		};		

	</script>	
	<script src="v_plus_client_data_array.js"></script>
</html>*/