import requests, json

# https://www.youtube.com/watch?v=-BG9lhTg6xY
token = "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"

query = """
query($owner: String!, $repo: String!) {
    repository(owner: $owner, name: $repo) {
    refs(refPrefix: "refs/tags/", first: 1, orderBy: { field: TAG_COMMIT_DATE, direction: DESC }) {
        edges {
        node {
            name
            target {
            ... on Tag {
                commitUrl
            }
            }
        }
        }
    }
    defaultBranchRef {
        target {
        ... on Commit {
            oid
        }
        }
    }
    }
}
"""

def fetch_latest_tag_and_commit(owner, repo):

    variables = {
        "owner": owner,
        "repo": repo
    }

    headers = {
        "Authorization": f"Bearer {token}"
    }

    response = requests.post(
        "https://api.github.com/graphql",
        headers=headers,
        json={"query": query, "variables": variables}
    )

    repository_data = response.json()["data"]["repository"]
    latest_tag = None
    latest_commit = repository_data["defaultBranchRef"]["target"]["oid"]

    if "refs" in repository_data and repository_data["refs"]["edges"]:
        latest_tag = repository_data["refs"]["edges"][0]["node"]["name"]

    return latest_tag, latest_commit


def main() -> int:

    with open("dependencies.json", "r") as file:
        dependencies = json.load(file)

    items = []

    for dependency in dependencies:

        name: str = dependency.get("name")
        github: str = dependency.get("github")

        owner, repo = github.split("/")
        tag, commit = fetch_latest_tag_and_commit(owner, repo)

        items.append({
            "name": name,
            "github": github,
            "tag": tag,
            "commit": commit
        })

    with open("dependencies.json", "w") as file:
        json.dump(items, file, indent=4)

if __name__ == "__main__":
    try: main()
    except KeyboardInterrupt:
        print("Operation aborted by user.")
        exit(-1)