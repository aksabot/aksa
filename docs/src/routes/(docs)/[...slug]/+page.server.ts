import { error } from "bosia";
import type { LoadEvent } from "bosia";
import { loadDoc } from "$lib/docs/content";
import { getLocale, stripLocale } from "$lib/docs/i18n";
import { buildSeoMeta } from "$lib/docs/seo";
import { readdirSync } from "fs";
import { join } from "path";

export const prerender = true;

export function entries(): Record<string, string>[] {
	const contentDir = join(process.cwd(), "content", "docs");
	const slugs: Record<string, string>[] = [];

	function scan(dir: string, prefix: string) {
		for (const entry of readdirSync(dir, { withFileTypes: true })) {
			if (entry.isDirectory()) {
				scan(join(dir, entry.name), prefix ? `${prefix}/${entry.name}` : entry.name);
			} else if (entry.name.endsWith(".md")) {
				const base = entry.name.replace(/\.md$/, "");
				const slug =
					base === "index" ? prefix : prefix ? `${prefix}/${base}` : base;
				if (slug) {
					// Root empty slug is the landing page, handled by its own route
					slugs.push({ slug });
				}
			}
		}
	}

	scan(contentDir, "");
	return slugs;
}

export async function load({ params, metadata }: LoadEvent) {
	const slug = params.slug || "";
	const locale = getLocale(slug);
	const bareSlug = stripLocale(slug);

	const page = metadata?.page ?? (await loadDoc(slug));

	if (!page) {
		error(404, "Not found");
	}

	return {
		html: page.html,
		headings: page.headings,
		frontmatter: page.frontmatter,
		currentSlug: bareSlug,
		locale,
	};
}

export async function metadata({ params }: { params: Record<string, string> }) {
	const slug = params.slug || "";
	const page = await loadDoc(slug);
	const title = page?.frontmatter?.title;
	const description = page?.frontmatter?.description;
	const locale = getLocale(slug);
	const bareSlug = stripLocale(slug);

	const fullTitle = title ? `${title} - Aksa Docs` : "Aksa Docs";
	const seo = buildSeoMeta({
		title: fullTitle,
		description:
			description ||
			"Documentation for Aksa — a tiny programming language for kids, in their own language.",
		slug: bareSlug,
		locale,
	});

	return {
		title: fullTitle,
		description: description || undefined,
		...seo,
		data: { page },
	};
}
