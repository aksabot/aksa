import { readFileSync, statSync, existsSync } from "fs";
import { join } from "path";
import { parseMarkdown, type DocPage } from "./markdown";

// Resolve relative to the docs project root (where `bun run dev|build` is invoked).
const contentDir = join(process.cwd(), "content", "docs");

interface CacheEntry {
	mtime: number;
	page: DocPage;
}

const cache = new Map<string, CacheEntry>();

/**
 * Check whether a doc page exists for a slug, without parsing it.
 * Mirrors loadDoc's resolution: `<slug>.md` or `<slug>/index.md`.
 */
export function docExists(slug: string): boolean {
	const s = slug || "index";
	if (s.includes("..")) return false;
	return existsSync(join(contentDir, `${s}.md`)) || existsSync(join(contentDir, `${s}/index.md`));
}

/**
 * Load a doc page by slug.
 * - "getting-started" → content/docs/getting-started.md
 * - "id/getting-started" → content/docs/id/getting-started.md
 */
export async function loadDoc(slug: string): Promise<DocPage | null> {
	if (!slug) slug = "index";

	// Prevent path traversal
	if (slug.includes("..")) return null;

	let filePath = join(contentDir, `${slug}.md`);

	// Fallback: treat slug as a directory and look for its index
	if (!existsSync(filePath)) {
		filePath = join(contentDir, `${slug}/index.md`);
	}

	if (!existsSync(filePath)) return null;

	const stat = statSync(filePath);
	const mtime = stat.mtimeMs;
	const key = filePath;

	const cached = cache.get(key);
	if (cached && cached.mtime === mtime) {
		return cached.page;
	}

	const raw = readFileSync(filePath, "utf-8");
	const page = await parseMarkdown(raw);

	cache.set(key, { mtime, page });
	return page;
}
