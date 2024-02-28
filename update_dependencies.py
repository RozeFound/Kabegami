from json import load as parse_json
from json import dump as write_json
from requests import Session

token = "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"

def main() -> int:

    session = Session()
    session.auth = ("Authorization: Bearer", token)

    with open("dependencies.json", "r") as file:
        dependencies = parse_json(file)

    items = []

    for dependency in dependencies:

        name = dependency.get("name")
        github = dependency.get("github")
        branch = dependency.get("branch")

        with session.get(f"https://api.github.com/repos/{github}/branches/{branch}") as response:
            commit = response.json().get("commit", {}).get("sha")

        with session.get(f"https://api.github.com/repos/{github}/releases/latest") as response:
            tag = response.json().get("tag_name") if response.status_code == 200 else commit

        items.append({
            "name": name,
            "github": github,
            "branch": branch,
            "tag": tag,
            "commit": commit
        })

    with open("dependencies.json", "w") as file:
        write_json(items, file, indent=4)

if __name__ == "__main__":
    try: main()
    except KeyboardInterrupt:
        print("Operation aborted by user.")
        exit(-1)