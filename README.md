# otus-algorithms
Homeworks for OTUS Algorithms

# Where are all sources?
They are in orphan branches. For each homework separate orphan branch will be created.
To create new branch just run:
```bash
git checkout --orphan hw-XX-description
git rm -rf .
echo "#Homework-XX description" > README.md
git add README.md
git commit -a -m "Initial Commit"
git push --set-upstream origin hw-XX-description
```

