<!DOCTYPE html>
<html>

<head>
<title>Index Page</title>
</head>

<body>
<h1>{{Title}}</h1>

<table>
	<tr>
		<th>Name</td>
		<th>Age</td>
	</tr>

	{{#IN_CA}}
	<tr>
		<td>{{Name}}</td>
		<td>{{Age}}</td>
	</tr>
	{{/IN_CA}}

</table>

</body>

</html>

