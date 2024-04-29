import asyncio, json, os

import httpx

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

async def get_updated_item(client: httpx.AsyncClient, dependency: dict[str, str]) -> dict[str, str]:

    name: str = dependency.get("name")
    github: str = dependency.get("github")

    owner, repo = github.split("/")

    json = {
        "query": query,
        "variables": {
            "owner": owner,
            "repo": repo
        }
    }

    response = await client.post("graphql", json=json)

    repository_data = response.json()["data"]["repository"]
    latest_tag = None
    latest_commit = repository_data["defaultBranchRef"]["target"]["oid"]

    if "refs" in repository_data and repository_data["refs"]["edges"]:
        latest_tag = repository_data["refs"]["edges"][0]["node"]["name"] 

    return {
        "name": name,
        "github": github,
        "tag": latest_tag,
        "commit": latest_commit
    }


async def main() -> int:

    if not (token := os.environ.get("GITHUB_TOKEN")):
        print("GITHUB_TOKEN environment variable not set.")
        exit(-1)

    with open("dependencies.json", "r") as file:
        dependencies = json.load(file)

    limits = httpx.Limits(max_connections=None, max_keepalive_connections=None)
    async with httpx.AsyncClient(limits=limits, http2=True) as client:
        client.base_url = "https://api.github.com"
        client.headers["Authorization"] = f"Bearer {token}"

        futures = [get_updated_item(client, d) for d in dependencies]
        items = await asyncio.gather(*futures)

    with open("dependencies.json", "w") as file:
        json.dump(items, file, indent=4)

if __name__ == "__main__":
    try: asyncio.run(main())
    except KeyboardInterrupt:
        print("Operation aborted by user.")
        exit(-1)