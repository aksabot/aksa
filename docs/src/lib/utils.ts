import { readFileSync } from "fs";
import { join } from "path";

// Product version = newest entry in the synced changelog (written by sync-docs.ts
// before every dev/build run), so the navbar badge always matches the changelog.
export function getVersion(): string {
	try {
		const changelog = readFileSync(
			join(process.cwd(), "content", "docs", "reference", "changelog.md"),
			"utf-8",
		);
		const m = changelog.match(/^## (\d+\.\d+\.\d+)/m);
		return m ? `v${m[1]}` : "";
	} catch {
		return "";
	}
}
