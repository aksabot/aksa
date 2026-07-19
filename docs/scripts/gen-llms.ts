/**
 * Generates public/llms.txt — an AI-agent index of the Aksa docs, built from
 * the same sidebar as the site so it stays in sync. public/ is copied into
 * dist/static at build time, so the file is served at
 * https://docs.aksabot.com/llms.txt.
 */

import { writeFileSync, mkdirSync } from "fs";
import { join } from "path";
import { sidebar, type NavGroup, type NavItem } from "../src/lib/docs/nav";

const BASE_URL = "https://docs.aksabot.com";
const outDir = join(process.cwd(), "public");

function lines(items: NavItem[], group: string): string[] {
	const out: string[] = [];
	for (const item of items) {
		if (item.slug !== undefined) {
			const url = item.slug === "" ? `${BASE_URL}/` : `${BASE_URL}/${item.slug}`;
			out.push(`- [${item.label}](${url})`);
		}
		if (item.children) out.push(...lines(item.children, group));
	}
	return out;
}

function section(group: NavGroup): string {
	return `## ${group.label}\n${lines(group.items, group.label).join("\n")}`;
}

const body =
	[
		"# Aksa",
		"> A tiny programming language for kids — keywords, builtins, and error messages all come from a locale dictionary (English and Indonesian today). Runs in the browser via WebAssembly and deploys to real ESP32 boards. Aksabot is the upcoming hardware kit built on the same language.",
		"",
		"Every page is also available in Indonesian under the /id/ prefix.",
		"",
		sidebar.map(section).join("\n\n"),
	].join("\n") + "\n";

mkdirSync(outDir, { recursive: true });
writeFileSync(join(outDir, "llms.txt"), body);
console.log("✓ llms.txt written");
