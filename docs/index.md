---
layout: page
title: "Home"
permalink: /
---

## Articles


{% assign posts = site.pages | where:"layout","post" | sort: "date" | reverse %}
{% for post in posts %}
- [{{ post.title }}]({{ post.url | relative_url }}){% if post.date %} — {{ post.date | date: "%Y-%m-%d" }}{% endif %}
  
  {{ post.content | strip_html | strip_newlines | truncate: 180 }}
{% endfor %}

## License

This site and the example code are licensed under the MIT License. See the [LICENSE](./LICENSE) file in the repository for details.


