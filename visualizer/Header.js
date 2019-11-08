import { html } from 'https://unpkg.com/lit-html@0.7.1/lib/lit-extended.js'

export default function Header(name) {
  return html`
  <header>${name}</header>
  `
}
