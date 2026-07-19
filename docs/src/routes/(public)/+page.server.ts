import { getVersion } from "$lib/utils";
import { buildSeoMeta } from "$lib/docs/seo";

export const prerender = true;

export async function load() {
	return { version: getVersion() };
}

export async function metadata() {
	const title = "Aksa Docs";
	const description =
		"Documentation for Aksa — a tiny programming language for kids, in their own language.";
	return {
		title,
		description,
		...buildSeoMeta({ title, description, slug: "", locale: "en" }),
	};
}
