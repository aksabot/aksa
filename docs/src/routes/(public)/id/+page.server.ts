import { getVersion } from "$lib/utils";
import { buildSeoMeta } from "$lib/docs/seo";

export const prerender = true;

export async function load() {
	return { version: getVersion() };
}

export async function metadata() {
	const title = "Aksa Docs";
	const description =
		"Dokumentasi Aksa — bahasa pemrograman kecil untuk anak, dalam bahasa mereka sendiri.";
	return {
		title,
		description,
		...buildSeoMeta({ title, description, slug: "", locale: "id" }),
	};
}
