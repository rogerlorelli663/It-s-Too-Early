default_start = 45

with open("/var/www/html/threshold_interface.html","r") as html_file:
    content = html_file.readlines();
    for i in range(len(content)):
        content[i] = content[i].strip()

    new_content = []

    for i in range(default_start):
        new_content.append(content[i])

    new_content.append("</table>")
    new_content.append("</body>")
    new_content.append("</html>")

    with open("/var/www/html/threshold_interface.html","w+") as html_file:
        for line in new_content:
            html_file.write(line)
            html_file.write('\n')

print(new_content)
print("Done")
