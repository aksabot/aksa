// Graded lesson content, one file per locale. Same order in every locale so
// switching languages keeps the place. Grouped basic (1-6) -> middle (7-11)
// -> advanced (12-15).

import { LESSONS_ID } from './lessons.id.ts';
import { LESSONS_EN } from './lessons.en.ts';

export interface Lesson {
  title: string;
  text: string;
  code: string;
}

export const LESSONS: Record<string, Lesson[]> = {
  id: LESSONS_ID,
  en: LESSONS_EN,
};
