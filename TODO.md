## Todo
- [ ] tab widget: one for settings, one for sessions, one for active session editing
- [ ] active session view would show timer, pause, end, and edit session details
- [ ] create new session button
- [ ] click on a session to set as active 

- [ ] create separate overlay that the main app can listen to and notify of active session changes

## Ideas
- Show me all sessions and the current session
- Sum up how many sessions I won/lost
- A session can simply be described by simple paragraph of what you'll do, maybe markdown
- Less rigidity in the sense of typing out thoughts and session goals...less due dates. Capture first, do later.
- Later: allow notes within a session, jot things down, all live in the same folder
  - It's like a todo.md
- Every session is based on week # - year - session x
- Every session is an individual document that lives on disk
- See which week of the year that I'm in
- See progress bar of how much of the year is done or percentage
- Show stats on how many sessions total I've won/lost
- Display a separate overlay window that shows me my active working item in center bottom in a pill
- Can commit to github for "sync" and even include an input url to auto-upload the data directory to github

- overlay can show the first line of the session doc (or separate input)
- review of session can be: `Win`, `Neutral`, or `Loss`

## Flow
- select location to save all your session data in markdown files
- data in the app parses the markdown file and uses it as the database, including the frontmatter

1. At the start of the day/session, set an intention by creating a session
2. Enter the top things to do this session in plain english (list or paragraph in markdown)
3. Click start session, and clock runs. Can pause, or stop.
4. When done for the session, select "Close session"...it will ask you to grade the session
5. View previous sessions in a list

## marketing
- local-only
- access files and metadata
- simple video demo OR flowy research-site page that has simple visuals

## Launch
- [ ] install via brew
- [ ] open source github
- [ ] hackernews "for developers with adhd"...purist site
