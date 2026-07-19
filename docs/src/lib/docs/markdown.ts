import { Marked } from "marked";
import matter from "gray-matter";
import { createHighlighter } from "shiki";
import { aksaGrammar } from "./aksa-grammar";

let highlighter: any = null;

export async function getHighlighter() {
	if (highlighter) return highlighter;
	highlighter = await createHighlighter({
		themes: ["github-light", "github-dark"],
		langs: [aksaGrammar as any, "bash", "c", "json"],
	});
	return highlighter;
}

export interface DocPage {
	frontmatter: Record<string, any>;
	html: string;
	headings: { id: string; text: string; level: number }[];
}

// Shared with scripts/gen-search-index.ts — search anchors must match rendered heading ids.
export function slugifyHeading(text: string): string {
	return text
		.toLowerCase()
		.replace(/<[^>]*>/g, "")
		.replace(/[^\w\s-]/g, "")
		.replace(/\s+/g, "-")
		.replace(/-+/g, "-")
		.trim();
}

export async function parseMarkdown(raw: string): Promise<DocPage> {
	const { data: frontmatter, content } = matter(raw);
	const hl = await getHighlighter();
	const headings: DocPage["headings"] = [];

	const marked = new Marked();

	marked.use({
		renderer: {
			heading({ text, depth }: { text: string; depth: number }) {
				const id = slugifyHeading(text);
				if (depth >= 2 && depth <= 4) {
					headings.push({ id, text: text.replace(/<[^>]*>/g, ""), level: depth });
				}
				return `<h${depth} id="${id}">${text}</h${depth}>`;
			},
			code({ text, lang }: { text: string; lang?: string }) {
				const language = lang || "text";
				try {
					const supported = ["aksa", "bash", "sh", "c", "json"];
					if (supported.includes(language)) {
						const mapped = language === "sh" ? "bash" : language;
						return hl.codeToHtml(text, {
							lang: mapped,
							themes: { light: "github-light", dark: "github-dark" },
							defaultColor: false,
						});
					}
				} catch (err) {
					console.error(`Syntax highlighting failed for language "${language}":`, err);
				}
				return `<pre><code class="language-${language}">${escapeHtml(text)}</code></pre>`;
			},
		},
	});

	const html = await marked.parse(content);

	return { frontmatter, html, headings };
}

function escapeHtml(s: string): string {
	return s
		.replace(/&/g, "&amp;")
		.replace(/</g, "&lt;")
		.replace(/>/g, "&gt;")
		.replace(/"/g, "&quot;");
}
