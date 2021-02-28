content_type = {
        "txt":"text/plain",
        "html":"text/html",
        "css": "text/css",
        "svg": "image/svg+xml",
        "js": "text/javascript",
        "jpg": "image/jpg",
        "jpeg": "image/jpeg",
        "jif": "image/gif",
        "ico": "image/x-icon"
}
print("keyhash for file extension")
for (key, value) in content_type.items():
    extenhash=0;
    for char in key:
        extenhash+=ord(char);
    print(f"case {extenhash}: return {value}")

print()
print("keyhash for sw.conf")
confkeys = [ "ip","port" ]

for key in confkeys:
    keyhash=0;
    for char in key:
        keyhash+=ord(char);
    print(f"// {key}")
    print(f"case {keyhash}: ")

